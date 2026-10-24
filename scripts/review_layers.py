"""Inventory every source image, vector shape and timeline, then export profiles.

The report records contextual inheritance: one reused sprite can belong to
different actors. Display-list slot number is never interpreted as distance.
"""
from pathlib import Path
import argparse,collections,json,sys,hashlib
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(Path(__file__).resolve().parent))
from lvl import parse
from timeline import operations,atlas

def export_profiles(root=ROOT):
    root=Path(root);profiles=json.loads((root/'docs/layer-profiles.json').read_text());rules=[];scenes=[]
    for level,scene in profiles['scenes'].items():
        roots=[scene['root'],*scene.get('extra_roots',[])];offset=len(rules)
        for group in scene['groups']:
            for name in group['names']:rules.append((level,-1,0,name,False,group))
            for kind,key in [(0,'images'),(1,'sprites')]:
                for ident in group[key]:rules.append((level,kind,ident,'',not group['detached'],group))
        scenes.extend((level,ident,scene['plane'],scene['relief'],offset,len(rules)-offset) for ident in roots)
    lines=['/* Generated from docs/layer-profiles.json by tools/review_layers.py. */',
           'static const struct {const char *level,*name;int kind;unsigned id;bool root_only;SmLayerState state;} layer_rules[]={']
    def f(value):return format(float(value),'.5f')+'f'
    for level,kind,ident,name,root_only,g in rules:
        lines.append('{'+','.join([json.dumps(level),json.dumps(name),str(kind),str(ident),'true' if root_only else 'false','{'+f(g['plane'])+','+f(g['relief'])+',SM_LAYER_'+g['group']+'}'])+'},')
    lines+=['};','static const struct {const char *level;unsigned root;float plane,relief;unsigned first,count;} scene_layers[]={']
    for name,ident,z,relief,first,count in scenes:lines.append('{'+f'"{name}",{ident},{f(z)},{f(relief)},{first},{count}'+'},')
    lines+=['};'];(root/'source/layer_profiles.inc').write_text('\n'.join(lines)+'\n',encoding='utf-8',newline='\n')
    return profiles

def review(game):
    profiles=export_profiles();data=Path(game)/'Data'
    main_hash=hashlib.sha256((data/'Main.swf').read_bytes()).hexdigest()
    if main_hash!='0ba23a7ca9c0d401a7439188c2407be5953e12a0c3ecbca5b792846aa0ff5b03':raise ValueError('Review profiles against this source version before exporting')
    levels=['Univerzal',*profiles['scenes']]
    source={level:parse(data/(level+'.lvl')) for level in levels};report={'source_main_sha256':main_hash,'source_levels':{n:d['sha256'] for n,d in source.items()},'policy':'explicit assemblies; animated descendants inherit their anchor; source slots are not depth','scenes':{}}
    total=collections.Counter();missing=[]
    for level,d in source.items():
        images={}
        for texture,blob in d['atlas_data'].items():images.update(atlas(blob,texture))
        movies={ident:operations(blob) for ident,blob in d['sprites'].items()};contexts=collections.defaultdict(set);seen=set()
        scene=profiles['scenes'].get(level);root_ids={scene['root'],*scene.get('extra_roots',[])} if scene else {0}
        names={d['strings'].get(op['name'],'') for movie in movies.values() for frame in movie['frames'] for op in frame if op['op']==10}
        declared={name for g in scene['groups'] for name in g['names']} if scene else set()
        # Start's curated moving groups and Rybar's bird are native special cases.
        declared.update({'Kopule'} if level=='Start' else {'Sup'} if level=='Rybar' else set())
        def policy(kind,ident,name,parent,inherited):
            if inherited:return inherited
            if level=='Start':
                special={610:'home planet assembly',611:'distant planet',615:'rocket hull/flame',617:'portrait animation',618:'orange effect assembly',619:'distant effect'}
                if kind==1 and ident in special:return special[ident]
                if kind==0 and ident in [1394,1397,1404,1405]:return 'home planet assembly'
                if kind==0 and 1310<=ident<=1336:return 'orange effect assembly'
                if kind==0 and ident in [1339,1341]:return 'portrait animation'
            if level=='Rybar' and kind==1 and ident==776:return 'bird/wing/passenger flight'
            if scene:
                for g in scene['groups']:
                    if name in g['names'] or ((not g['detached'] and parent in root_ids or g['detached']) and ident in g['images' if kind==0 else 'sprites'] if kind in (0,1) else False):return g['label']
            return 'connected scenery assembly' if parent in root_ids else ''
        def visit(kind,ident,name,parent,inherited,root_name,path):
            own=policy(kind,ident,name,parent,inherited);key=(kind,ident)
            contexts[key].add(root_name+': '+(own or 'scene plane'))
            guard=(key,own,root_name)
            if guard in seen or key in path:return
            seen.add(guard)
            if kind==1 and ident in movies:
                for frame in movies[ident]['frames']:
                    for op in frame:
                        if op['op'] in (1,5,10):visit(op['flags']&7,op['id'],d['strings'].get(op.get('name',-1),''),ident,own,root_name,path|{key})
        for name,ident in d['names'].items():
            if ident in movies:visit(1,ident,'',-1,'',name,set())
        if level=='Univerzal':visit(1,0,'',-1,'GUI/credits plane','common',set())
        objects=[]
        for kind,table in [(0,images),(1,movies),(2,d['shapes'])]:
            for ident,value in sorted(table.items()):
                obj={'kind':['image','timeline','vector'][kind],'id':ident,'contexts':sorted(contexts[(kind,ident)]),'status':'contextually assigned' if contexts[(kind,ident)] else 'not reachable from exported named roots'}
                if kind==0:obj['atlas']=value
                if kind==1:
                    obj.update(frames=value['total'],labels=[{'frame':f,'label':d['strings'].get(label,'')} for f,label in value['labels']],color_or_alpha_changes=sum(bool(op.get('flags',0)&192) for frame in value['frames'] for op in frame))
                objects.append(obj)
        unresolved=sorted(names-declared)
        if unresolved:missing.append((level,unresolved))
        counts={'images':len(images),'timelines':len(movies),'vectors':len(d['shapes']),'timeline_frames':sum(m['total'] for m in movies.values()),'named_objects':len(names),'reviewed_groups':len(scene['groups']) if scene else 1,'reachable_definitions':sum(bool(o['contexts']) for o in objects),'unused_definitions':sum(not o['contexts'] for o in objects)}
        total.update(counts);report['scenes'][level]={'counts':counts,'unassigned_names':unresolved,'groups':scene['groups'] if scene else [],'objects':objects}
    report['totals']=dict(total);(ROOT/'docs/layer-inventory.json').write_text(json.dumps(report,indent=2)+'\n')
    lines=['# Complete layer review','','All source definitions and frames were structurally inspected; semantic assemblies and their contact masks were reviewed separately. Shared definitions inherit their actual actor context; unreachable source assets are listed separately. Depth placement is an artistic draft, not recovered physical distance.','','| Scene | Images | Timelines | Vectors | Frames | Named objects | Groups | Unused definitions |','| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |']
    for name,entry in report['scenes'].items():
        c=entry['counts'];lines.append('| '+name+' | '+' | '.join(str(c[k]) for k in ['images','timelines','vectors','timeline_frames','named_objects','reviewed_groups','unused_definitions'])+' |')
    lines+=['','## Placement decisions','','* Connected photo tiles remain on one base plane so seams do not open.','* Actor poses, wings, fingers, mouths and carried objects inherit one assembly.','* Shadows, ropes, knobs, printed labels and their supporting objects share contact planes.','* Translucent smoke, light and swarm frames do not gain depth from alpha or frame index.','* Existing rocket and bird approach curves remain active. Other assemblies use stable depths unless an authored approach cue has been reviewed.','* Runtime fades and credits use the screen plane.','* Original paint order is retained; depth profiles never sort transparent objects.','','`layer-profiles.json` is the editable source. `layer-inventory.json` contains every definition and its contextual assignments. Inherited animation parts are reviewed and deliberately grouped, rather than assigned arbitrary independent depth.']
    for name,entry in report['scenes'].items():
        if not entry['groups']:continue
        lines+=['','## '+name,'','| Assembly | Resting plane | Relief gain | Members |','| --- | ---: | ---: | --- |']
        for g in entry['groups']:
            members=g['names']+['image '+str(i) for i in g['images']]+['sprite '+str(i) for i in g['sprites']]
            lines.append('| '+g['label']+' | '+str(g['plane'])+' | '+str(g['relief'])+' | '+', '.join(members)+' |')
    lines+=['','## Animation handoffs','',*['* '+s for s in profiles.get('animated_handoffs',[])],'','## Contact-mask checks','','`check_layer_contacts.py` compares first-pose alpha silhouettes and vector masks for the six interactive scenes. This detected and corrected the tree-hollow cover, branch/bark joint and bridge rock/railing contact. It is an initial-pose geometric check, not a claim that every animation frame has been visually reviewed on hardware.']
    (ROOT/'docs/LAYER_REVIEW.md').write_text('\n'.join(lines)+'\n',encoding='utf-8',newline='\n')
    if missing:raise ValueError('Named objects without reviewed policies: '+repr(missing))
    print('Complete layer inventory:',dict(total))
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--game',default=r'C:/Program Files (x86)/Steam/steamapps/common/Samorost 1');a=p.parse_args();review(a.game)
