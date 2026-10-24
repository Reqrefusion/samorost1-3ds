"""Translate the inspected scene handlers from the locally owned AIR game.



This deliberately small translator handles this game's known code, not arbitrary

ActionScript. Native adapters replace Starling display, audio and touch APIs.

Unsupported decompiler expressions stop generation. Original files stay in work/.

"""

from pathlib import Path

import re,hashlib,json



ROOT=Path(__file__).resolve().parents[1]

SCENES=['Start','Lyzar','Rybar','Vcely','Strom','Vodarna','Strojovna']



def body(source,name):

    match=re.search(r'function '+name+r'\([^)]*\)\s*:\s*\w+\s*\{',source)

    if not match:return None

    start=match.end();end=start;depth=1

    while depth:

        depth+=(source[end]=='{')-(source[end]=='}');end+=1

    return source[start:end-1]



def native(code):

    code=code.replace('this.','').replace('===','==')

    code=re.sub(r'\s+as (?:TimelineMovie|TimelineSprite|DisplayObjectContainer|DisplayObject|ISensor)', '',code)

    code=code.replace('Vector.<Touch>','TouchList')

    code=code.replace('getTouches(this,','getTouches(nullptr,')

    code=code.replace('Math.min','std::min<float>').replace('Math.max','std::max<float>').replace('Math.random()','(float(rand())/RAND_MAX)')

    # Numeric and string casts are handled explicitly below.

    code=re.sub(r'(enableSensors|disableSensors)\(\[([^]]*)\]\)',r'\1({\2})',code)

    code=re.sub(r'setTimeout\((\w+)\.play,([^)]*)\)',r'delayPlay(\1,\2)',code)

    # Discard only Starling's non-interactive outer-layer insertion; the native

    # root already contains both the room and its separately animated planet.

    code=re.sub(r'^\s*(?:mParent\.addChildAt|addChildAt)\([^;]*;', '',code,flags=re.M)

    code=re.sub(r'(\w+)\.touchable\s*=\s*([^;]+);',r'\1.setTouchable(\2);',code)

    code=code.replace('._mVisible','.visible')

    code=code.replace('.mVisible','.visible')

    # Property assignments become checked methods on generation-aware handles.

    for prop in ['visible','enabled','fps']:

        code=re.sub(r'(\w+|TimelineMovie\(param1\))\.'+prop+r'\s*=\s*([^;]+);',r'\1.'+prop+r'(\2);',code)

    for prop in ['mParent','mName','name','currentFrame','isPlaying','visible','enabled']:

        code=re.sub(r'\.'+prop+r'\b(?!\s*\()', '.'+prop+'()',code)

    code=re.sub(r'\.mChildren\.indexOf\(','.indexOf(',code)

    code=re.sub(r'switch\(([^\n]+)\)',lambda m:'switch(hash('+m[1]+'))' if any(x in m[1] for x in ['param2','.name()', '.mName()']) else m[0],code)

    code=re.sub(r'case "([^"\n]+)":',r'case H("\1"):',code)

    code=code.replace('|| _loc4_ && !_loc4_.isPlaying()', '|| (_loc4_ && !_loc4_.isPlaying())')

    code=code.replace('bStrom = sprKrytkaSosakStrom.visible(true);','bStrom = sprKrytkaSosakStrom.visible(true); [[fallthrough]];').replace('bStrom = sprKrytkaSosakStrom.visible(false);','bStrom = sprKrytkaSosakStrom.visible(false); [[fallthrough]];')

    code=code.replace('case H("Zebrik"):\n               case H("Skret"):', '[[fallthrough]];\n               case H("Zebrik"):\n               case H("Skret"):')

    declarations={}

    def declare(m):

        name,typ,initializer=m[1],m[2],m[3]

        typ={'Number':'float','Boolean':'bool','String':'String','int':'int','uint':'unsigned','*':'Movie'}.get(typ,typ)

        declarations[name]=typ

        return name+' = '+initializer+';' if initializer is not None else ''

    code=re.sub(r'var (\w+):([\w*]+)(?:\s*=\s*([^;]+))?;',declare,code)

    code=re.sub(r'\bnull\b','Movie()',code)

    if '§§' in code or 'Starling' in code or 'Tween' in code or 'var ' in code:

        raise ValueError('Untranslated code: '+code)

    for name,typ in declarations.items():

        if typ=='String':code=code.replace(name+' = Movie();',name+' = String();')

    return '\n'.join(f' {typ} {name}{{}};' for name,typ in declarations.items())+'\n'+code



def generate():

    sources={n:(ROOT/'work/actionscript/scripts/scenes'/f'{n}.as').read_text(encoding='utf-8-sig') for n in SCENES}

    fields={}

    for s in sources.values():

        for name,typ,initial in re.findall(r'(?:public|protected|private) var (\w+):(\w+)(?:\s*=\s*([^;]+))?;',s):

            fields[name]=(typ,initial)

    out=['// Generated from inspected local ActionScript; regenerate with tools/port_rules.py.']

    for name,(typ,initial) in fields.items():

        cpp={'Number':'float','Boolean':'bool','String':'String','int':'int'}.get(typ,'Movie')

        out.append(f'static {cpp} {name};')

    out+=['static Movie _layerHvezdy,_layerBoky;','static void reset_fields(){']

    for name,(typ,initial) in fields.items():

        cpp={'Number':'float','Boolean':'bool','String':'String','int':'int'}.get(typ,'Movie')

        out.append(f' {name}={initial if initial else cpp+"()"};')

    out+=['}','static bool onBtn(String);','static void testKnobs();']

    for n,s in sources.items():

        methods=['onTouchSensor'] if n=='Start' else ['onAnimLabel','onTouchSensor','onAddNamedObject']

        for method in methods:

            code=body(s,method)

            if code is None:continue

            if n=='Vodarna' and method=='onAnimLabel':

                # FFDec stack reconstruction is invalid for these two reorders.

                # Both move Skret behind the named object in _layerScene.

                code=re.sub(r'case "SkretDole":.*?(?=enableSensors)', 'case "SkretDole":\n_layerScene.addChildAt(animSkret,_layerScene.indexOf(animMravenec));\n',code,flags=re.S)

                code=re.sub(r'case "DoPozadi":.*?break;', 'case "DoPozadi":\n_layerScene.addChildAt(animSkret,_layerScene.indexOf(animDvere));\nbreak;',code,flags=re.S)

            if n=='Strojovna' and method=='onTouchSensor':

                # Tween-only statements are replaced with the native light state.

                code=re.sub(r'^\s*var \w+:Tween = null;','',code,flags=re.M)

                code=re.sub(r'^\s*var \w+:Starling = Starling;','',code,flags=re.M)

                code=re.sub(r'^\s*\(starling[^\n]+;','',code,flags=re.M)

                code=re.sub(r'^\s*_loc3_ = new Tween[^;]+;','',code,flags=re.M)

                code=code.replace('_loc3_.fadeTo(0);','fade(qTma,0,0.5f);').replace('_loc3_.fadeTo(0.92);','fade(qTma,0.92f,0.3f);')

            if n=='Strojovna' and method=='onAnimLabel':code=code.replace('_LayerPlaneta.play();','_LayerPlaneta.visible = true;\n_LayerPlaneta.play();')

            signature='TouchEvent param1' if method=='onTouchSensor' else 'Movie param1,String param2' if method=='onAnimLabel' else 'Movie param1'

            out.append(f'static void {n}_{method}({signature}){{'+native(code)+'\n}')

    for n,method,signature in [('Strojovna','onBtn','String param1'),('Vodarna','testKnobs','')]:

        out.append(f'static {"bool" if method=="onBtn" else "void"} {method}({signature}){{'+native(body(sources[n],method))+'\n}')

    out.append('static void bind_scene(){switch(level){')

    for index,n in enumerate(SCENES):

        init=body(sources[n],'onInit');out.append(f'case {index}:')

        # Actor pointers are original named-object bindings, never positional guesses.

        for line in init.splitlines():

            if re.search(r'^\s*\w+ = _layerScene\.getChildByName',line):out.append(native(line))

            if re.search(r'^\s*(?:anim|spr)\w+\.(?:stop\(\)|touchable\s*=|visible\s*=)',line):out.append(native(line))

            if 'addQuadSensor(_layerScene,' in line:out.append(line.strip())

            if re.search(r'^\s*disableSensors\(\[',line):out.append(native(line))

            if 'Global.playAtmoLoop(' in line:out.append(line.strip())

        if n=='Strojovna':out+=['mGlobal.disableAllSensors();mGlobal.enableSensor("Vypinac");']

        out.append('break;')

    out+=['}}']

    # Methods are used before definition only by the dispatch following this include.

    dest=ROOT/'source/rules.inc';dest.write_text('\n'.join(out)+'\n',encoding='utf-8')

    manifest={str(p.relative_to(ROOT/'work/actionscript/scripts')):hashlib.sha256(p.read_bytes()).hexdigest() for p in (ROOT/'work/actionscript/scripts/scenes').glob('*.as') if p.stem in SCENES}

    (ROOT/'docs/rules-provenance.json').write_text(json.dumps({'game':'Samorost 1 AIR 1.13.2','main_swf_sha256':'0ba23a7ca9c0d401a7439188c2407be5953e12a0c3ecbca5b792846aa0ff5b03','source_handlers':manifest,'generated_sha256':hashlib.sha256(dest.read_bytes()).hexdigest()},indent=2)+'\n')

    print('Generated inspected rules:',dest)



if __name__=='__main__':generate()

