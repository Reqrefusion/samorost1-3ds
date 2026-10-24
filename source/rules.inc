// Generated from inspected local ActionScript; regenerate with tools/port_rules.py.
static Movie _layerScene;
static Movie animHvezdy;
static Movie sprKopule;
static Movie sprPlaneta;
static Movie sprCredits;
static Movie qTma;
static Movie animCedulka;
static Movie animSberac1;
static Movie animSberac2;
static Movie animSberac3;
static Movie animSberac4;
static Movie animSberac5;
static Movie animSberac6;
static Movie animSberac7;
static Movie animKurak;
static Movie animTabak;
static Movie animKour;
static Movie animCudl;
static Movie animLyzar;
static int baf;
static bool odemceno;
static int smer;
static bool zavazi;
static Movie animSup;
static Movie animVcela;
static Movie animKoza1;
static Movie animKoza2;
static Movie animKoza3;
static Movie animKostricka;
static Movie animVoda;
static Movie animRybar;
static Movie animMloci;
static bool bu1;
static bool bu2;
static int mloku;
static Movie animVcely;
static Movie animKvety;
static Movie animSosak;
static Movie animSnek;
static Movie animStrom;
static Movie animSkret;
static Movie animZarovka;
static Movie animSvetlo;
static Movie animGondola;
static Movie animGondolaObj;
static Movie animVypinac;
static Movie animZasek;
static Movie sprKrytkaSosakStrom;
static bool bZarovka;
static bool bStrom;
static bool bKvety;
static bool bOtevrena;
static bool bVcelyKlik;
static bool bVytah;
static Movie animDatel;
static Movie animCerv1;
static Movie animCerv2;
static Movie animCerv3;
static Movie animCerv4;
static Movie animSova;
static Movie animSovaPozice;
static Movie animLano;
static Movie animVeverka;
static Movie animGramec;
static Movie animDutina;
static Movie animVetev;
static bool bCerv1;
static bool bCerv2;
static bool bCerv3;
static bool bCerv4;
static bool bDutina;
static bool bGramec;
static int numDeska;
static String hraje;
static String strSova;
static int skretFrame;
static Movie animRozvadec;
static Movie animMravec;
static Movie animMravenec;
static Movie animDvere;
static Movie animKlika;
static Movie animHouba1;
static Movie animHouba2;
static Movie animKnob1;
static Movie animKnob2;
static Movie animKnob3;
static Movie animKnob4;
static Movie animKnob5;
static Movie animKnob6;
static bool bKlikaZavrena;
static bool bOdemceno;
static Movie _LayerPlaneta;
static Movie animKour0;
static Movie animKour1;
static Movie animKour2;
static Movie animKour3;
static Movie animRafika1;
static Movie animRafika2;
static Movie animZebrik;
static Movie animKolo;
static Movie animKoloStin;
static Movie animDioda;
static Movie animPaka;
static int numQ;
static bool bZebrik;
static bool bTma;
static Movie _layerHvezdy,_layerBoky;
static void reset_fields(){
 _layerScene=Movie();
 animHvezdy=Movie();
 sprKopule=Movie();
 sprPlaneta=Movie();
 sprCredits=Movie();
 qTma=Movie();
 animCedulka=Movie();
 animSberac1=Movie();
 animSberac2=Movie();
 animSberac3=Movie();
 animSberac4=Movie();
 animSberac5=Movie();
 animSberac6=Movie();
 animSberac7=Movie();
 animKurak=Movie();
 animTabak=Movie();
 animKour=Movie();
 animCudl=Movie();
 animLyzar=Movie();
 baf=0;
 odemceno=false;
 smer=1;
 zavazi=true;
 animSup=Movie();
 animVcela=Movie();
 animKoza1=Movie();
 animKoza2=Movie();
 animKoza3=Movie();
 animKostricka=Movie();
 animVoda=Movie();
 animRybar=Movie();
 animMloci=Movie();
 bu1=false;
 bu2=false;
 mloku=0;
 animVcely=Movie();
 animKvety=Movie();
 animSosak=Movie();
 animSnek=Movie();
 animStrom=Movie();
 animSkret=Movie();
 animZarovka=Movie();
 animSvetlo=Movie();
 animGondola=Movie();
 animGondolaObj=Movie();
 animVypinac=Movie();
 animZasek=Movie();
 sprKrytkaSosakStrom=Movie();
 bZarovka=false;
 bStrom=false;
 bKvety=false;
 bOtevrena=false;
 bVcelyKlik=false;
 bVytah=false;
 animDatel=Movie();
 animCerv1=Movie();
 animCerv2=Movie();
 animCerv3=Movie();
 animCerv4=Movie();
 animSova=Movie();
 animSovaPozice=Movie();
 animLano=Movie();
 animVeverka=Movie();
 animGramec=Movie();
 animDutina=Movie();
 animVetev=Movie();
 bCerv1=false;
 bCerv2=false;
 bCerv3=false;
 bCerv4=false;
 bDutina=false;
 bGramec=false;
 numDeska=0;
 hraje=String();
 strSova="Sova0";
 skretFrame=1;
 animRozvadec=Movie();
 animMravec=Movie();
 animMravenec=Movie();
 animDvere=Movie();
 animKlika=Movie();
 animHouba1=Movie();
 animHouba2=Movie();
 animKnob1=Movie();
 animKnob2=Movie();
 animKnob3=Movie();
 animKnob4=Movie();
 animKnob5=Movie();
 animKnob6=Movie();
 bKlikaZavrena=true;
 bOdemceno=false;
 _LayerPlaneta=Movie();
 animKour0=Movie();
 animKour1=Movie();
 animKour2=Movie();
 animKour3=Movie();
 animRafika1=Movie();
 animRafika2=Movie();
 animZebrik=Movie();
 animKolo=Movie();
 animKoloStin=Movie();
 animDioda=Movie();
 animPaka=Movie();
 numQ=30;
 bZebrik=false;
 bTma=true;
}
static bool onBtn(String);
static void testKnobs();
static void Start_onTouchSensor(TouchEvent param1){ ISensor _loc3_{};
 TouchList _loc2_{};
 String _loc4_{};

         _loc3_ = ISensor(param1.currentTarget);
         _loc2_ = param1.getTouches(nullptr,"began");
         if(_loc2_.length > 0)
         {
            _loc3_.name();
         }
         _loc2_ = param1.getTouches(nullptr,"ended");
         if(_loc3_.enabled() && _loc2_.length > 0 && !bWasMoving)
         {
            _loc4_ = _loc3_.name();
            if("Domek" == _loc4_)
            {
               _loc3_.enabled(false);
               _layerScene.play();
            }
         }
      
}
static void Lyzar_onAnimLabel(Movie param1,String param2){ TimelineMovie _loc3_{};
 String _loc4_{};
 DisplayObjectContainer _loc5_{};
 Movie _loc6_{};
 String _loc7_{};
 String _loc8_{};
 String _loc9_{};
 String _loc10_{};
 String _loc11_{};

         _loc3_ = Movie();
         _loc4_ = String();
         switch(hash(param2))
         {
            case H("Sber"):
               _loc5_ = param1;
               _loc6_ = _loc5_.mParent();
               _loc4_ = _loc6_.mName();
               _loc3_ = _layerScene.getChildByName(_loc4_);
               if(_loc3_)
               {
                  _loc3_.gotoAndStop(1);
               }
               _loc7_ = _loc4_;
               mGlobal.enableSensor(_loc7_);
               break;
            case H("Sedi1"):
               if(baf == 3)
               {
                  animKurak.gotoAndStop(5);
               }
               else
               {
                  animKurak.gotoAndStop(1);
                  _loc8_ = "RukaDymka";
                  mGlobal.enableSensor(_loc8_);
               }
               break;
            case H("Odhodil"):
            case H("Sedi2"):
               _loc9_ = "Klic";
               mGlobal.enableSensor(_loc9_);
               animKurak.gotoAndStop(6);
               break;
            case H("Vyhasina"):
               animKour.play();
               break;
            case H("Odemceno"):
               odemceno = !odemceno;
               break;
            case H("CudlON"):
               disableSensors({"Kolo","Lano1","Lano2"});
               if(animLyzar.currentFrame() == 1)
               {
                  animLyzar.gotoAndStop(2);
                  focusTo(0,0.8087514936203745,0.40350877192982465);
               }
               else if(animLyzar.currentFrame() == 4)
               {
                  animLyzar.gotoAndStop(5);
                  focusTo(0,1.266407661666812,0.40350877192982465);
               }
               break;
            case H("CudlOFF"):
               animCudl.gotoAndStop(1);
               _loc10_ = "Cudl";
               mGlobal.enableSensor(_loc10_);
               break;
            case H("LanoDole"):
               animLyzar.gotoAndStop(1);
               enableSensors({"Kolo","Lano1","Lano2"});
               break;
            case H("LanoNasazeny"):
               animLyzar.gotoAndStop(4);
               break;
            case H("LyzarCeka"):
               _loc11_ = "Lyzar";
               mGlobal.enableSensor(_loc11_);
               break;
            case H("Smer1"):
               smer = 1;
               break;
            case H("Smer2"):
               smer = 2;
               break;
            case H("Smer3"):
               smer = 3;
               break;
            case H("Zavazi"):
               zavazi = true;
               break;
            case H("Odchod"):
               mGlobal.zatmeni(true,0.6,"Rybar","Lyzar",true);
         }
      
}
static void Lyzar_onTouchSensor(TouchEvent param1){ ISensor _loc3_{};
 TouchList _loc2_{};
 String _loc4_{};

         _loc3_ = ISensor(param1.currentTarget);
         _loc2_ = param1.getTouches(nullptr,"began");
         if(_loc2_.length > 0)
         {
            _loc4_ = _loc3_.name();
            if("Cudl" == _loc4_)
            {
               animCudl.gotoAndStop(odemceno ? 3 : 2);
               _loc3_.enabled(false);
            }
         }
         _loc2_ = param1.getTouches(nullptr,"ended");
         if(_loc3_.enabled() && _loc2_.length > 0 && !bWasMoving)
         {
            switch(hash(_loc3_.name()))
            {
               case H("Cedulka"):
                  animCedulka.play();
                  break;
               case H("Sberac1"):
                  animSberac1.gotoAndStop(2);
                  _loc3_.enabled(false);
                  zavazi = false;
                  break;
               case H("Sberac2"):
                  animSberac2.gotoAndStop(2);
                  _loc3_.enabled(false);
                  break;
               case H("Sberac3"):
                  animSberac3.gotoAndStop(2);
                  _loc3_.enabled(false);
                  break;
               case H("Sberac4"):
                  animSberac4.gotoAndStop(2);
                  _loc3_.enabled(false);
                  break;
               case H("Sberac5"):
                  animSberac5.gotoAndStop(2);
                  _loc3_.enabled(false);
                  break;
               case H("Sberac6"):
                  animSberac6.gotoAndStop(2);
                  _loc3_.enabled(false);
                  break;
               case H("Sberac7"):
                  animSberac7.gotoAndStop(2);
                  _loc3_.enabled(false);
                  break;
               case H("RukaDymka"):
                  if(baf < 3)
                  {
                     baf = baf + 1;
                     animKurak.gotoAndStop(1 + baf);
                     animTabak.play();
                     _loc3_.enabled(false);
                  }
                  break;
               case H("Klic"):
                  _loc3_.enabled(false);
                  animKurak.gotoAndStop(7);
                  break;
               case H("Lano1"):
               case H("Lano2"):
               case H("Kolo"):
                  if(animLyzar.currentFrame() == 1)
                  {
                     disableSensors({"Kolo","Lano1","Lano2"});
                     animLyzar.gotoAndStop(3);
                  }
                  break;
               case H("Lyzar"):
                  if(smer < 3)
                  {
                     focusTo(0,0.13565636798128086,0.40350877192982465);
                  }
                  switch(smer - 1)
                  {
                     case 0:
                        animLyzar.gotoAndStop(6);
                        break;
                     case 1:
                        animLyzar.gotoAndStop(7);
                        break;
                     default:
                        if(zavazi && animLyzar.currentFrame() == 8)
                        {
                           TimelineMovie(animLyzar.getChildAt(0)).play();
                           return;
                        }
                        animLyzar.gotoAndStop(zavazi ? 8 : 9);
                  }
                  _loc3_.enabled(false);
            }
         }
      
}
static void Rybar_onAnimLabel(Movie param1,String param2){ String _loc4_{};
 String _loc5_{};
 String _loc6_{};
 String _loc7_{};

         switch(hash(param2))
         {
            case H("Bu1"):
               animKoza1.gotoAndStop(1);
               break;
            case H("Bu2"):
               animKoza2.gotoAndStop(1);
               break;
            case H("KozaSpi"):
               animKoza3.gotoAndStop(1);
               _loc4_ = "Koza3";
               mGlobal.enableSensor(_loc4_);
               break;
            case H("Sedi"):
               animLyzar.gotoAndStop(2);
               _loc5_ = "Lyzar";
               mGlobal.enableSensor(_loc5_);
               break;
            case H("Vyskok"):
               animLyzar.gotoAndStop(4);
               break;
            case H("NaSkok"):
               animLyzar.gotoAndStop(4);
               _loc6_ = "Lyzar";
               mGlobal.disableSensor(_loc6_);
               break;
            case H("TestSkok"):
               if(animSup.currentFrame() == 4 && TimelineMovie(animSup.getChildAt(0)).currentFrame() > 140)
               {
                  TimelineMovie(param1).stop();
               }
               break;
            case H("Skocil"):
               if(animSup.currentFrame() == 4 && TimelineMovie(animSup.getChildAt(0)).currentFrame() > 140)
               {
                  TimelineMovie(param1).gotoAndPlay(13);
               }
               break;
            case H("Leti"):
               animLyzar.visible(false);
               break;
            case H("VodaPryc"):
               animVoda.play();
               break;
            case H("RybaDoma"):
               TimelineMovie(param1).stop();
               animKour.play();
               animMloci.play();
               break;
            case H("Kouri"):
               if(mloku < 6)
               {
                  animKour.stop();
               }
               break;
            case H("Snezena"):
               animKostricka.play();
               break;
            case H("KostVenku"):
               animSup.gotoAndStop(2);
               break;
            case H("Priletl"):
               animSup.gotoAndStop(3);
               animKostricka.visible(false);
               _loc7_ = "Sup";
               mGlobal.enableSensor(_loc7_);
               break;
            case H("OdletVcely"):
               mGlobal.zatmeni(true,0.6,"Vcely","Rybar",true);
               break;
            case H("MlokDole"):
               TimelineMovie(param1.mParent()).gotoAndStop(3);
               break;
            case H("Mlok1"):
            case H("Mlok2"):
            case H("Mlok3"):
            case H("Mlok4"):
            case H("Mlok5"):
            case H("Mlok6"):
               mGlobal.removeSensorByName(param2);
         }
      
}
static void Rybar_onTouchSensor(TouchEvent param1){ DisplayObject _loc3_{};
 TimelineMovie _loc4_{};
 ISensor _loc5_{};
 TouchList _loc2_{};
 DisplayObject _loc6_{};

         _loc3_ = Movie();
         _loc4_ = Movie();
         _loc5_ = ISensor(param1.currentTarget);
         _loc2_ = param1.getTouches(nullptr,"began");
         if(_loc2_.length > 0)
         {
            _loc5_.name();
         }
         _loc2_ = param1.getTouches(nullptr,"ended");
         if(_loc5_.enabled() && _loc2_.length > 0 && !bWasMoving)
         {
            switch(hash(_loc5_.name()))
            {
               case H("Koza1"):
                  animKoza1.gotoAndStop(2);
                  break;
               case H("Koza2"):
                  animKoza2.gotoAndStop(2);
                  break;
               case H("Koza3"):
                  animKoza3.gotoAndStop(2);
                  _loc5_.enabled(false);
                  break;
               case H("Splavek"):
               case H("Prut"):
                  animRybar.gotoAndStop(2);
                  _loc5_.enabled(false);
                  break;
               case H("Sup"):
                  animSup.nextFrame();
                  _loc5_.enabled(false);
                  break;
               case H("Lyzar"):
                  animLyzar.gotoAndStop(animSup.currentFrame() > 1 ? 4 : 3);
                  break;
               case H("Mlok1"):
               case H("Mlok2"):
               case H("Mlok3"):
               case H("Mlok4"):
               case H("Mlok5"):
               case H("Mlok6"):
                  _loc3_ = _loc5_;
                  _loc6_ = _loc3_;
                  _loc4_ = _loc6_.mParent();
                  if(_loc4_)
                  {
                     mloku = mloku + 1;
                     _loc3_.removeFromParent();
                     _loc4_.gotoAndStop(2);
                     if(mloku == 6)
                     {
                        animKour.play();
                        setFocusAC(3);
                     }
                  }
            }
         }
      
}
static void Rybar_onAddNamedObject(Movie param1){ DisplayObjectContainer _loc2_{};
 DisplayObject _loc3_{};
 DisplayObject _loc4_{};
 DisplayObject _loc5_{};
 String _loc7_{};
 DisplayObjectContainer _loc6_{};

         _loc2_ = Movie();
         _loc3_ = param1;
         switch(hash(_loc3_.mName()))
         {
            case H("Mlok1"):
            case H("Mlok2"):
            case H("Mlok3"):
            case H("Mlok4"):
            case H("Mlok5"):
            case H("Mlok6"):
               _loc2_ = param1;
               if(_loc2_)
               {
                  
                  addQuadSensor(_loc2_,(_loc4_ = param1).mName(),-304,-10,73,97);
                  _loc5_ = param1;
                  _loc7_ = _loc5_.mName();
                  mGlobal.enableSensor(_loc7_);
                  _loc6_ = _loc2_;
                  _loc6_.mParent().setTouchable(true);
                  _loc2_.setTouchable(true);
               }
         }
      
}
static void Vcely_onAnimLabel(Movie param1,String param2){ String _loc5_{};
 TimelineMovie _loc4_{};
 String _loc6_{};
 String _loc7_{};
 String _loc8_{};
 String _loc9_{};
 String _loc10_{};
 String _loc11_{};

         switch(hash(param2))
         {
            case H("Kytka1"):
               TimelineMovie(param1).fps(25);
               bVcelyKlik = true;
               break;
            case H("Kytka0"):
               TimelineMovie(param1).fps(25);
               bVcelyKlik = false;
               break;
            case H("Strom4"):
               bStrom = sprKrytkaSosakStrom.visible(true); [[fallthrough]];
            case H("Strom0"):
            case H("Strom1"):
            case H("Strom2"):
            case H("Strom3"):
               TimelineMovie(param1).stop();
               _loc5_ = param2;
               mGlobal.enableSensor(_loc5_);
               break;
            case H("SosakTam"):
               animKvety.play();
               break;
            case H("SosakZpet"):
               animSosak.gotoAndStop(1);
               break;
            case H("Snek"):
               
               TimelineMovie((_loc4_ = animSnek).mParent()).play();
               animSnek.gotoAndStop(1);
               addQuadSensor(animSnek,"Snek",-87,-82,76,73);
               break;
            case H("SnekPryc"):
               removeQuadSensor("Snek");
               break;
            case H("Pousti"):
               animSkret.gotoAndStop(2);
               break;
            case H("Sedi"):
               _loc6_ = "Skret";
               mGlobal.enableSensor(_loc6_);
               animSkret.gotoAndStop(3);
               break;
            case H("Zamavat"):
               animSkret.gotoAndStop(4);
               break;
            case H("Kvety"):
               bKvety = true;
               break;
            case H("Kuk"):
               animZarovka.gotoAndStop(2);
               break;
            case H("ZarKouka"):
               if(animVcely.currentFrame() > 2)
               {
                  animZarovka.gotoAndStop(3);
                  _loc7_ = "Dira";
                  mGlobal.enableSensor(_loc7_);
               }
               break;
            case H("ZarZpet"):
               animZarovka.gotoAndStop(1);
               break;
            case H("Vcely1"):
               animVcely.gotoAndStop(1);
               _loc8_ = "Dira";
               mGlobal.enableSensor(_loc8_);
               break;
            case H("Vcely2"):
               animVcely.gotoAndStop(4);
               _loc9_ = "Kytka";
               mGlobal.enableSensor(_loc9_);
               break;
            case H("Zarovka"):
               bZarovka = true;
               break;
            case H("DoGo"):
               animGondolaObj.play();
               TimelineMovie(param1).stop();
               break;
            case H("Gondola"):
               animGondola.play();
               break;
            case H("Nastup"):
               animGondolaObj.stop();
               _loc10_ = "DoGondoly";
               mGlobal.enableSensor(_loc10_);
               bOtevrena = true;
               break;
            case H("Skret1"):
               animSkret.gotoAndStop(1);
               break;
            case H("VGondole"):
               animGondola.play();
               break;
            case H("GondolaNahore"):
               TimelineMovie(param1).stop();
               _loc11_ = "Gondola";
               mGlobal.enableSensor(_loc11_);
               break;
            case H("Odchod"):
               mGlobal.zatmeni(true,0.6,"Strom","Vcely",true);
         }
      
}
static void Vcely_onTouchSensor(TouchEvent param1){ ISensor _loc3_{};
 TouchList _loc2_{};
 String _loc4_{};
 String _loc6_{};
 String _loc7_{};
 TimelineMovie _loc5_{};

         _loc3_ = ISensor(param1.currentTarget);
         _loc2_ = param1.getTouches(nullptr,"began");
         if(_loc2_.length > 0)
         {
            _loc4_ = _loc3_.name();
            if("Vypinac" == _loc4_)
            {
               animVypinac.gotoAndStop(2);
               Global.playSound("Vcely/Tlacitko");
            }
         }
         _loc2_ = param1.getTouches(nullptr,"ended");
         if(_loc3_.enabled() && _loc2_.length > 0)
         {
            _loc4_ = _loc3_.name();
            if("Vypinac" == _loc4_)
            {
               animVypinac.gotoAndStop(1);
               Global.playSound("Vcely/Tlacitko",0.1);
               if(bZarovka)
               {
                  if(!bVytah)
                  {
                     bVytah = true;
                     animSvetlo.play();
                     weekFocusTo(0,1.9999758882616128,std::max<float>(_focusZoom,0.4109942993294651),5);
                  }
               }
            }
            if(!bWasMoving)
            {
               switch(hash(_loc3_.name()))
               {
                  case H("Strom4"):
                     if(animSosak.currentFrame() == 3)
                     {
                        if(animZasek)
                        {
                           animZasek.play();
                        }
                        break;
                     }
                     bStrom = sprKrytkaSosakStrom.visible(false); [[fallthrough]];
                  case H("Strom0"):
                  case H("Strom1"):
                  case H("Strom2"):
                  case H("Strom3"):
                     _loc3_.enabled(false);
                     animStrom.play();
                     weekFocusTo(0,std::min<float>(_focusY,-0.8042646537937352),_focusZoom);
                     break;
                  case H("DiraSosak"):
                     if(animStrom.currentFrame() > 92 && animStrom.currentFrame() <= 121)
                     {
                        bStrom = true;
                     }
                     animSosak.gotoAndStop(bStrom ? 3 : 2);
                     _loc3_.enabled(!bStrom);
                     if(bStrom)
                     {
                        focusTo(0,0.5178721858779902,std::min<float>(_focusZoom,0.29811848551717884));
                        setFocusAC(5);
                     }
                     break;
                  case H("Dira"):
                     if(animVcely.currentFrame() < 3)
                     {
                        if(animKvety.currentFrame() > 2)
                        {
                           bKvety = true;
                        }
                        if(bKvety)
                        {
                           animVcely.gotoAndStop(3);
                           weekFocusTo(0,1.9999758882616128,std::min<float>(_focusZoom,0.4109942993294651),4);
                        }
                        else
                        {
                           animVcely.gotoAndStop(2);
                           weekFocusTo(0,1.9999758882616128,_focusZoom);
                        }
                     }
                     else
                     {
                        animZarovka.gotoAndStop(4);
                        focusTo(0,1.9999758882616128,std::max<float>(_focusZoom,0.4109942993294651));
                        setFocusAC(0.5);
                     }
                     _loc3_.enabled(false);
                     break;
                  case H("Skret"):
                     animSkret.gotoAndStop(bOtevrena ? 5 : 4);
                     _loc3_.enabled(false);
                     _loc6_ = "DoGondoly";
                     mGlobal.disableSensor(_loc6_);
                     break;
                  case H("Gondola"):
                     animGondolaObj.play();
                     _loc3_.enabled(false);
                     break;
                  case H("DoGondoly"):
                     animSkret.gotoAndStop(5);
                     _loc3_.enabled(false);
                     _loc7_ = "Skret";
                     mGlobal.disableSensor(_loc7_);
                     break;
                  case H("Kytka"):
                     if(bVcelyKlik && animVcely.currentFrame() == 4)
                     {
                        TimelineMovie(animVcely.getChildAt(0)).speedUp(120,2,60);
                        bVcelyKlik = false;
                        Global.playSound("Vcely/Muchy",0.3 + 0.4 * (float(rand())/RAND_MAX));
                     }
                     break;
                  case H("Lebka"):
                     Global.playSound("Vcely/LebkaKlik",0.5 + 0.5 * (float(rand())/RAND_MAX));
                     break;
                  case H("Snek"):
                     removeQuadSensor("Snek");
                     
                     TimelineMovie((_loc5_ = animSnek).mParent()).stop();
                     animSnek.gotoAndStop(2);
               }
            }
         }
      
}
static void Vcely_onAddNamedObject(Movie param1){ DisplayObject _loc2_{};
 TimelineMovie _loc3_{};

         _loc2_ = param1;
         switch(hash(_loc2_.mName()))
         {
            case H("GondolaObj"):
               animGondolaObj = param1;
               break;
            case H("Zasek"):
               animZasek = param1;
               break;
            case H("Snek"):
               animSnek = param1;
               if(animSnek)
               {
                  addQuadSensor(animSnek,"Snek",-87,-82,76,73);
                  _loc3_ = animSnek;
                  _loc3_.mParent().setTouchable(true);
               }
         }
      
}
static void Strom_onAnimLabel(Movie param1,String param2){ String _loc3_{};
 String _loc4_{};
 String _loc5_{};
 String _loc6_{};
 String _loc7_{};
 String _loc8_{};
 String _loc9_{};
 String _loc10_{};
 String _loc11_{};
 String _loc12_{};
 String _loc13_{};
 String _loc14_{};

         switch(hash(param2))
         {
            case H("TestSova"):
               if(!bGramec)
               {
                  animSova.gotoAndStop(3);
               }
               break;
            case H("TestGramec"):
               if(!bGramec)
               {
                  animGramec.gotoAndStop(1);
               }
               break;
            case H("Cerv1"):
            case H("Cerv2"):
            case H("Cerv3"):
            case H("Cerv4"):
               _loc3_ = param2;
               mGlobal.enableSensor(_loc3_);
               break;
            case H("Cerv1Pryc"):
               if(bCerv1)
               {
                  animCerv1.stop();
               }
               _loc4_ = "Cerv1";
               mGlobal.disableSensor(_loc4_);
               break;
            case H("Cerv2Pryc"):
               if(bCerv2)
               {
                  animCerv2.stop();
               }
               _loc5_ = "Cerv2";
               mGlobal.disableSensor(_loc5_);
               break;
            case H("Cerv3Pryc"):
               if(bCerv3)
               {
                  animCerv3.stop();
               }
               _loc6_ = "Cerv3";
               mGlobal.disableSensor(_loc6_);
               break;
            case H("Cerv4Pryc"):
               if(bCerv4)
               {
                  animCerv4.stop();
               }
               _loc7_ = "Cerv4";
               mGlobal.disableSensor(_loc7_);
               break;
            case H("DatelZpet"):
               animDatel.gotoAndStop(1);
               if(bCerv1 && bCerv2 && !bCerv3 && !bCerv4)
               {
                  weekFocusTo(0,-1.3643518003459112,_focusZoom,16);
               }
               else if(bCerv1 && bCerv2 && bCerv3 && bCerv4)
               {
                  weekFocusTo(0,1.9998908546656644,_focusZoom);
               }
               break;
            case H("DutinaOn"):
               bDutina = true;
               enableSensors({"Veverka","Gramec"});
               TimelineMovie(param1).stop();
               break;
            case H("Zacpane"):
               animVeverka.gotoAndStop(1);
               if(bDutina)
               {
                  _loc8_ = "Veverka";
                  mGlobal.enableSensor(_loc8_);
               }
               break;
            case H("Sedi"):
               if(animVeverka.currentFrame() == 3)
               {
                  animVeverka.gotoAndStop(5);
               }
               else
               {
                  animVeverka.gotoAndStop(4);
                  if(bDutina)
                  {
                     _loc9_ = "Veverka";
                     mGlobal.enableSensor(_loc9_);
                  }
               }
               break;
            case H("Deska"):
               animVeverka.gotoAndStop(6 + numDeska);
               break;
            case H("GramecON"):
               bGramec = true;
               numDeska = numDeska + 1;
               if(numDeska > 3)
               {
                  numDeska = 1;
               }
               switch(numDeska - 1)
               {
                  case 0:
                     animGramec.gotoAndStop(2);
                     hraje = "Strom/Jazz";
                     break;
                  case 1:
                     animGramec.gotoAndStop(3);
                     hraje = "Strom/Klasika";
                     break;
                  case 2:
                     animGramec.gotoAndStop(4);
                     hraje = "Strom/DnB";
                     animSova.gotoAndStop(4);
                     _loc10_ = "Sova0";
                     mGlobal.disableSensor(_loc10_);
               }
               Global.playAtmoLoop(hraje,1,0.3);
               if(animVeverka.currentFrame() == 5)
               {
                  animVeverka.gotoAndStop(6 + numDeska);
               }
               break;
            case H("StopMusic"):
               Global.stopAtmoLoop(hraje,0.3);
               animGramec.gotoAndStop(1);
               bGramec = false;
               break;
            case H("NaSkoky"):
               animSovaPozice.gotoAndStop(2);
               break;
            case H("Tanci"):
               TimelineMovie(param1).stop();
               break;
            case H("Spi"):
               _loc11_ = strSova;
               mGlobal.enableSensor(_loc11_);
               animSova.gotoAndStop(2);
               break;
            case H("Sova1"):
            case H("Sova2"):
            case H("Sova3"):
               strSova = param2;
               _loc12_ = param2;
               mGlobal.enableSensor(_loc12_);
               TimelineMovie(param1).stop();
               break;
            case H("GondolaZasek"):
               TimelineMovie(animSovaPozice.getChildAt(0)).play();
               animVetev.play();
               if(animSkret)
               {
                  animSkret.play();
               }
               break;
            case H("GondolaVysi"):
               if(animSkret)
               {
                  skretFrame = animSkret.currentFrame();
               }
               animGondola.gotoAndStop(3);
               _loc13_ = "Gondola";
               mGlobal.enableSensor(_loc13_);
               break;
            case H("GondolaZhup"):
               animLano.play();
               animVetev.play();
               if(animSkret)
               {
                  skretFrame = animSkret.currentFrame();
               }
               animGondola.gotoAndStop(4);
               if(animSkret)
               {
                  animSkret.play();
               }
               break;
            case H("GondolaPad"):
               _loc14_ = "Gondola";
               mGlobal.disableSensor(_loc14_);
               animLano.play();
               if(animSkret)
               {
                  skretFrame = animSkret.currentFrame();
               }
               animGondola.gotoAndStop(5);
               focusTo(0,-1.9999958404397518,0.40350877192982465);
               setFocusAC(0.5);
               zoomEnabled = false;
               break;
            case H("onZatmeni"):
               if(animSkret)
               {
                  skretFrame = animSkret.currentFrame();
               }
               animGondola.gotoAndStop(2);
               animLano.play();
               break;
            case H("Odchod"):
               mGlobal.zatmeni(true,0.6,"Vodarna","Strom",false);
         }
      
}
static void Strom_onTouchSensor(TouchEvent param1){ ISensor _loc3_{};
 TouchList _loc2_{};

         _loc3_ = ISensor(param1.currentTarget);
         _loc2_ = param1.getTouches(nullptr,"began");
         if(_loc2_.length > 0)
         {
            _loc3_.name();
         }
         _loc2_ = param1.getTouches(nullptr,"ended");
         if(_loc3_.enabled() && _loc2_.length > 0 && !bWasMoving)
         {
            switch(hash(_loc3_.name()))
            {
               case H("Datel"):
                  if(animDatel.currentFrame() == 1)
                  {
                     animDatel.gotoAndStop(2);
                  }
                  break;
               case H("Cerv1"):
                  if(animDatel.currentFrame() == 1)
                  {
                     bCerv1 = true;
                     animDatel.gotoAndStop(3);
                  }
                  break;
               case H("Cerv2"):
                  if(animDatel.currentFrame() == 1)
                  {
                     bCerv2 = true;
                     animDatel.gotoAndStop(4);
                  }
                  break;
               case H("Cerv3"):
                  if(animDatel.currentFrame() == 1)
                  {
                     bCerv3 = true;
                     animDatel.gotoAndStop(5);
                  }
                  break;
               case H("Cerv4"):
                  if(animDatel.currentFrame() == 1)
                  {
                     bCerv4 = true;
                     animDatel.gotoAndStop(6);
                  }
                  break;
               case H("Dutina"):
                  if(!animDutina.isPlaying())
                  {
                     if(bDutina)
                     {
                        disableSensors({"Veverka","Gramec"});
                        bDutina = false;
                     }
                     animDutina.play();
                  }
                  break;
               case H("Veverka"):
                  if(bCerv1 && bCerv2 && bCerv3 && bCerv4)
                  {
                     switch(animVeverka.currentFrame() - 1)
                     {
                        case 0:
                           animVeverka.gotoAndStop(3);
                           break;
                        case 3:
                           animVeverka.gotoAndStop(5);
                           break;
                        case 6:
                        case 7:
                           animVeverka.gotoAndStop(6);
                           break;
                        case 8:
                           animVeverka.gotoAndStop(10);
                           numDeska = 0;
                     }
                     weekFocusTo(0,0.7039165485004851,std::min<float>(_focusZoom,0.40350877192982465));
                  }
                  else if(animVeverka.currentFrame() == 1)
                  {
                     animVeverka.gotoAndStop(2);
                  }
                  break;
               case H("Gramec"):
                  if(bCerv1 && bCerv2 && bCerv3 && bCerv4)
                  {
                     switch(animVeverka.currentFrame() - 1)
                     {
                        case 0:
                           animVeverka.gotoAndStop(3);
                           break;
                        case 3:
                           animVeverka.gotoAndStop(5);
                           break;
                        case 6:
                        case 7:
                           animVeverka.gotoAndStop(6);
                           break;
                        case 8:
                           animVeverka.gotoAndStop(10);
                           numDeska = 0;
                     }
                  }
                  else if(animVeverka.currentFrame() == 1)
                  {
                     animVeverka.gotoAndStop(2);
                  }
                  break;
               case H("Sova0"):
                  if(animSova)
                  {
                     animSova.gotoAndStop(numDeska == 3 ? 1 : 3);
                     _loc3_.enabled(false);
                  }
                  break;
               case H("Sova1"):
               case H("Sova2"):
               case H("Sova3"):
                  if(numDeska == 3)
                  {
                     TimelineMovie(animSovaPozice.getChildAt(0)).play();
                     if(_loc3_.name() == "Sova3")
                     {
                        animVetev.play();
                        mGlobal.disableAllSensors();
                     }
                     _loc3_.enabled(false);
                  }
                  else
                  {
                     animSova.gotoAndStop(3);
                  }
                  break;
               case H("Gondola"):
                  if(animSkret)
                  {
                     skretFrame = animSkret.currentFrame();
                  }
                  if(animGondola.currentFrame() == 3)
                  {
                     animGondola.gotoAndStop(4);
                  }
            }
         }
      
}
static void Strom_onAddNamedObject(Movie param1){ DisplayObject _loc2_{};

         _loc2_ = param1;
         switch(hash(_loc2_.mName()))
         {
            case H("Sova"):
               animSova = param1;
               if(numDeska != 3)
               {
                  animSova.gotoAndStop(2);
               }
               break;
            case H("Skret"):
               animSkret = param1;
               if(skretFrame > 0 && animSkret)
               {
                  animSkret.gotoAndPlay(skretFrame);
               }
         }
      
}
static void Vodarna_onAnimLabel(Movie param1,String param2){ String _loc5_{};
 String _loc6_{};
 String _loc7_{};
 String _loc10_{};

         switch(hash(param2))
         {
            case H("Brana"):
               animMravenec.gotoAndStop(bKlikaZavrena ? 2 : 3);
               break;
            case H("Klika1"):
            case H("Klika2"):
               _loc5_ = param2;
               mGlobal.enableSensor(_loc5_);
               break;
            case H("TestMravec"):
               if(animMravec.currentFrame() == 1)
               {
                  animMravec.gotoAndStop(3);
               }
               break;
            case H("MravenecPryc"):
               animMravenec.gotoAndStop(1);
               TimelineMovie(animMravenec.getChildAt(0)).stop();
               break;
            case H("Snedl"):
               animMravec.gotoAndStop(1);
               TimelineMovie(animMravenec.getChildAt(0)).play();
               break;
            case H("MravenecDole"):
               animMravenec.gotoAndStop(1);
               break;
            case H("Gondola"):
               _loc6_ = "GondolaTlac";
               mGlobal.enableSensor(_loc6_);
               break;
            case H("GondolaDohup"):
               animGondola.gotoAndStop(2);
               break;
            case H("GondolaStop"):
               TimelineMovie(param1).stop();
               break;
            case H("Open"):
               _loc7_ = "Gondola";
               mGlobal.enableSensor(_loc7_);
               break;
            case H("SkretDole"):
_layerScene.addChildAt(animSkret,_layerScene.indexOf(animMravenec));
enableSensors({"Houba1","Houba2"});
               focusTo(0,1.8433235269477032e-7,0.40350877192982465);
               setFocusAC(3);
               break;
            case H("DoPozadi"):
_layerScene.addChildAt(animSkret,_layerScene.indexOf(animDvere));
break;
            case H("Stoji"):
               animSkret.gotoAndStop(3);
               enableSensors({"Schody1","Schody2","Skret"});
               break;
            case H("Vyplivl"):
               animMravec.gotoAndStop(1);
               animSkret.visible(true);
               enableSensors({"Schody1","Schody2","Skret"});
               break;
            case H("Nahore"):
               animSkret.gotoAndStop(6);
               enableSensors({"Rozvadec","Dvere"});
               break;
            case H("Houba1"):
               animHouba1.play();
               Global.playSound("Vodarna/Houba1");
               break;
            case H("Houba2"):
               animHouba2.play();
               Global.playSound("Vodarna/Houba1");
               break;
            case H("RozvadecDetail"):
               enableSensors({"Knob1","Knob2","Knob3","Knob4","Knob5","Knob6"});
               _loc10_ = "Rozvadec";
               mGlobal.disableSensor(_loc10_);
               break;
            case H("DvereOpen"):
               bOdemceno = true;
               animDvere.gotoAndStop(2);
               TimelineMovie(animDvere.getChildAt(0)).play();
               break;
            case H("onZatmeni"):
               break;
            case H("Odchazi"):
               TimelineMovie(animDvere.getChildAt(0)).play();
               mGlobal.zatmeni(true,0.6,"Strojovna","Vodarna",true);
         }
      
}
static void Vodarna_onTouchSensor(TouchEvent param1){ TimelineMovie _loc3_{};
 ISensor _loc4_{};
 TouchList _loc2_{};

         _loc3_ = Movie();
         _loc4_ = ISensor(param1.currentTarget);
         _loc2_ = param1.getTouches(nullptr,"began");
         if(_loc2_.length > 0)
         {
            _loc4_.name();
         }
         _loc2_ = param1.getTouches(nullptr,"ended");
         if(_loc4_.enabled() && _loc2_.length > 0 && !bWasMoving)
         {
            switch(hash(_loc4_.name()))
            {
               case H("Houba1"):
               case H("Houba2"):
               case H("Houba3"):
                  Global.playSound("Vodarna/Houba1");
                  _loc3_ = _layerScene.getChildByName(_loc4_.name());
                  if(_loc3_)
                  {
                     _loc3_.play();
                  }
                  break;
               case H("Houba4"):
               case H("Houba5"):
               case H("Houba6"):
                  Global.playSound("Vodarna/Houba2");
                  _loc3_ = _layerScene.getChildByName(_loc4_.name());
                  if(_loc3_)
                  {
                     _loc3_.play();
                  }
                  break;
               case H("Houba7"):
               case H("Houba8"):
               case H("Houba9"):
               case H("Houba10"):
                  Global.playSound("Vodarna/Houba3");
                  _loc3_ = _layerScene.getChildByName(_loc4_.name());
                  if(_loc3_)
                  {
                     _loc3_.play();
                  }
                  break;
               case H("Klika1"):
                  _loc4_.enabled(false);
                  bKlikaZavrena = false;
                  animKlika.play();
                  break;
               case H("Klika2"):
                  _loc4_.enabled(false);
                  bKlikaZavrena = true;
                  animKlika.play();
                  break;
               case H("GondolaTlac"):
                  animGondola.gotoAndStop(3);
                  _loc4_.enabled(false);
                  break;
               case H("Gondola"):
                  _loc4_.enabled(false);
                  animSkret.gotoAndStop(2);
                  break;
               case H("Skret"):
                  animSkret.gotoAndStop(4);
                  _loc4_.enabled(false);
                  break;
               case H("Schody1"):
               case H("Schody2"):
                  disableSensors({"Schody1","Schody2","Skret"});
                  if(animSkret.currentFrame() == 3)
                  {
                     if(animMravec.currentFrame() == 1)
                     {
                        animMravec.gotoAndStop(2);
                        animSkret.visible(false);
                     }
                     else
                     {
                        animSkret.gotoAndStop(5);
                        focusTo(0,1.99972583264002,std::max<float>(_focusZoom,0.40350877192982465));
                        setFocusAC(2);
                     }
                  }
                  break;
               case H("Dvere"):
                  if(animDvere.currentFrame() == 1)
                  {
                     TimelineMovie(animDvere.getChildAt(0)).play();
                  }
                  if(animDvere.currentFrame() > 1)
                  {
                     _loc4_.enabled(false);
                     animSkret.gotoAndStop(7);
                  }
                  break;
               case H("Rozvadec"):
                  animRozvadec.play();
                  break;
               case H("Knob1"):
                  animKnob1.gotoAndStop(animKnob1.currentFrame() == 1 ? 2 : 1);
                  testKnobs();
                  break;
               case H("Knob2"):
                  animKnob2.gotoAndStop(animKnob2.currentFrame() == 1 ? 2 : 1);
                  testKnobs();
                  break;
               case H("Knob3"):
                  animKnob3.gotoAndStop(animKnob3.currentFrame() == 1 ? 2 : 1);
                  testKnobs();
                  break;
               case H("Knob4"):
                  animKnob4.gotoAndStop(animKnob4.currentFrame() == 1 ? 2 : 1);
                  testKnobs();
                  break;
               case H("Knob5"):
                  animKnob5.gotoAndStop(animKnob5.currentFrame() == 1 ? 2 : 1);
                  testKnobs();
                  break;
               case H("Knob6"):
                  animKnob6.gotoAndStop(animKnob6.currentFrame() == 1 ? 2 : 1);
                  testKnobs();
            }
         }
      
}
static void Vodarna_onAddNamedObject(Movie param1){ DisplayObject _loc2_{};

         _loc2_ = param1;
         switch(hash(_loc2_.mName()))
         {
            case H("Gondola"):
               animGondola = param1;
               break;
            case H("Knob1"):
               animKnob1 = param1;
               break;
            case H("Knob2"):
               animKnob2 = param1;
               break;
            case H("Knob3"):
               animKnob3 = param1;
               break;
            case H("Knob4"):
               animKnob4 = param1;
               break;
            case H("Knob5"):
               animKnob5 = param1;
               break;
            case H("Knob6"):
               animKnob6 = param1;
         }
      
}
static void Strojovna_onAnimLabel(Movie param1,String param2){ TimelineSprite _loc3_{};
 String _loc4_{};
 TimelineMovie _loc5_{};
 String _loc6_{};

         switch(hash(param2))
         {
            case H("TestKour"):
               _loc3_ = _layerScene;
               if(!_loc3_.visible())
               {
                  TimelineMovie(param1).gotoAndPlay(1);
               }
               break;
            case H("Zebrik"):
               if(!bTma)
               {
                  _loc4_ = "Zebrik";
                  mGlobal.enableSensor(_loc4_);
               }
               bZebrik = true;
               TimelineMovie(param1).stop();
               break;
            case H("Sedi"):
               animSkret.gotoAndStop(1);
               break;
            case H("Nahore"):
               animPaka.visible(false);
               TimelineMovie(param1).stop();

               _loc5_ = _LayerPlaneta;

               _layerHvezdy.visible(true);
               _layerBoky.visible(false);
               _layerScene.visible(false);
               _LayerPlaneta.visible(true);
_LayerPlaneta.play();
               Global.stopLastLongChanels(0.1);
               Global.volumeAtmoLoop("Strojovna/AtmoStrojovna",0,0.5);
               Global.playAtmoLoop("Univerzal/Detektivka",1,0.5);
               break;
            case H("Hotovo"):
               TimelineMovie(param1).stop();
               animKolo.play();
               animKoloStin.play();
               mGlobal.disableAllSensors();
               animKour0.gotoAndPlay("TestKour");
               delayPlay(animKour1,1500);
               delayPlay(animKour2,2500);
               delayPlay(animKour3,3000);
               delayPlay(animKour1,4000);
               break;
            case H("KoloOtoceny"):
               Global.stopLastLongChanels(0.1);
               _layerBoky.visible(false);
               _layerHvezdy.visible(true);
               _LayerPlaneta.visible(true);
               _layerScene.visible(false);
               _LayerPlaneta.visible(true);
_LayerPlaneta.play();
               Global.volumeAtmoLoop("Strojovna/AtmoStrojovna",0,0.5);
               Global.playAtmoLoop("Univerzal/Detektivka",1,0.5);
               break;
            case H("Planeta1"):
               TimelineMovie(param1).stop();
               _layerBoky.visible(true);
               _layerHvezdy.visible(false);
               _LayerPlaneta.visible(false);
               _layerScene.visible(true);
               animSkret.nextFrame();
               break;
            case H("Planeta2"):
               TimelineMovie(param1).stop();
               _layerBoky.visible(true);
               _layerHvezdy.visible(false);
               _LayerPlaneta.visible(false);
               focusTo(0,0,0,false,false,true);
               focusTo(0,-1.9873569738177608,0.4034666395673402,true,false,true);
               _layerScene.visible(true);
               animRafika2.gotoAndStop(2);
               break;
            case H("Paka0"):
               _loc6_ = "Paka";
               mGlobal.disableSensor(_loc6_);
               break;
            case H("NovyKurz"):
               Global.stopLastLongChanels(0.1);
               mGlobal.zatmeni(true,0.6,"Start","Strojovna",false);
         }
      
}
static void Strojovna_onTouchSensor(TouchEvent param1){ TimelineMovie _loc4_{};
 ISensor _loc5_{};
 TouchList _loc2_{};
 String _loc11_{};
 String _loc12_{};
 String _loc13_{};
 TimelineMovie _loc10_{};

         _loc4_ = Movie();

         _loc5_ = ISensor(param1.currentTarget);
         _loc2_ = param1.getTouches(nullptr,"began");
         if(_loc2_.length > 0)
         {
            _loc5_.name();
         }
         _loc2_ = param1.getTouches(nullptr,"ended");
         if(_loc5_.enabled() && _loc2_.length > 0 && !bWasMoving)
         {
            switch(hash(_loc5_.name()))
            {
               case H("Vypinac"):
                  if(!animVypinac.isPlaying())
                  {


                     if(animVypinac.currentFrame() == 1)
                     {
                        bTma = false;

                        fade(qTma,0,0.5f);

                        enableSensors({"Plus","Minus","Zebr","Kour1","Kour2","Kour3","Paka"});
                        if(bZebrik)
                        {
                           if(animSkret.currentFrame() < 4 && !bTma)
                           {
                              enableSensors({"Zebrik","Paka"});
                           }
                        }
                        else
                        {
                           _loc11_ = "Skret";
                           mGlobal.enableSensor(_loc11_);
                        }
                     }
                     else
                     {
                        bTma = true;

                        fade(qTma,0.92f,0.3f);

                        mGlobal.disableAllSensors();
                     }
                     _loc12_ = "Vypinac";
                     mGlobal.enableSensor(_loc12_);
                     animVypinac.play();
                  }
                  break;
               case H("Plus"):
                  Global.playSound("Strojovna/Tlacitko1");
                  if(onBtn(_loc5_.name()))
                  {
                     _loc4_ = animRafika1.getChildAt(0);
                     if(animRafika1.currentFrame() == 1 || (_loc4_ && !_loc4_.isPlaying()))
                     {
                        switch(numQ)
                        {
                           case 30:
                              animRafika1.gotoAndStop(2);
                              numQ = 40;
                              break;
                           case 40:
                              animRafika1.gotoAndStop(3);
                              numQ = 50;
                              break;
                           case 50:
                              animRafika1.gotoAndStop(4);
                              numQ = 60;
                              break;
                           case 60:
                              animRafika1.gotoAndStop(5);
                              numQ = 10;
                              break;
                           case 10:
                              animRafika1.gotoAndStop(6);
                              numQ = 20;
                              break;
                           case 20:
                              animRafika1.gotoAndStop(7);
                              numQ = 30;
                        }
                     }
                  }
                  break;
               case H("Minus"):
                  Global.playSound("Strojovna/Tlacitko1");
                  if(onBtn(_loc5_.name()))
                  {
                     _loc4_ = animRafika1.getChildAt(0);
                     if(animRafika1.currentFrame() == 1 || (_loc4_ && !_loc4_.isPlaying()))
                     {
                        switch(numQ)
                        {
                           case 30:
                              animRafika1.gotoAndStop(8);
                              numQ = 20;
                              break;
                           case 20:
                              animRafika1.gotoAndStop(9);
                              numQ = 10;
                              break;
                           case 10:
                              animRafika1.gotoAndStop(10);
                              numQ = 60;
                              break;
                           case 60:
                              animRafika1.gotoAndStop(11);
                              numQ = 50;
                              break;
                           case 50:
                              animRafika1.gotoAndStop(12);
                              numQ = 40;
                              break;
                           case 40:
                              animRafika1.gotoAndStop(13);
                              numQ = 30;
                        }
                     }
                  }
                  break;
               case H("Zebr"):
                  Global.playSound("Strojovna/Tlacitko2");
                  if(onBtn(_loc5_.name()))
                  {
                     if(numQ == 50 && animZebrik.currentFrame() == 1)
                     {
                        animZebrik.play();
                        animSkret.gotoAndStop(3);
                        weekFocusTo(0,1.9986007525922491,0.4035087719298246);
                     }
                     else if(numQ != 50 && animZebrik.currentFrame() == 87 && animSkret.currentFrame() < 4)
                     {
                        animZebrik.play();
                        animSkret.gotoAndStop(3);
                        _loc13_ = "Zebrik";
                        mGlobal.disableSensor(_loc13_);
                        bZebrik = false;
                     }
                  }
                  break;
               case H("Kour1"):
                  Global.playSound("Strojovna/Tlacitko3");
                  if(onBtn(_loc5_.name()))
                  {
                     animKour1.play();
                  }
                  break;
               case H("Kour2"):
                  Global.playSound("Strojovna/Tlacitko3");
                  if(onBtn(_loc5_.name()))
                  {
                     animKour2.play();
                  }
                  break;
               case H("Kour3"):
                  Global.playSound("Strojovna/Tlacitko3");
                  if(onBtn(_loc5_.name()))
                  {
                     animKour3.play();
                  }
                  break;
               case H("Paka"):
                  _loc10_ = animPaka;
                  if(_loc10_.visible())
                  {
                     animPaka.play();
                     if(!bZebrik || animSkret.currentFrame() == 4)
                     {
                        break;
                     }
                  }
               [[fallthrough]];
               case H("Zebrik"):
               case H("Skret"):
                  if(bZebrik)
                  {
                     animSkret.gotoAndStop(4);
                     disableSensors({"Skret","Zebrik","Vypinac","Zebr","Plus","Minus","Kour1","Kour2","Kour3"});
                     focusTo(0,1.9986007525922491,0.4035087719298246);
                     setFocusAC(3);
                  }
                  else
                  {
                     animSkret.gotoAndStop(2);
                  }
            }
         }
      
}
static bool onBtn(String param1){ TimelineMovie _loc2_{};

         weekFocusTo(0,-1.9972642953178215,0.4035087719298246,4);
         _loc2_ = _layerScene.getChildByName("Btn" + param1);
         if(_loc2_ && _loc2_.currentFrame() == 1)
         {
            _loc2_.play();
            return true;
         }
         return false;
      
}
static void testKnobs(){

         Global.playSound("Vodarna/Otocenipojistek");
         if(animKnob1.currentFrame() == 1 && animKnob2.currentFrame() == 1 && animKnob3.currentFrame() == 2 && animKnob4.currentFrame() == 2 && animKnob5.currentFrame() == 2 && animKnob6.currentFrame() == 2)
         {
            animRozvadec.play();
            disableSensors({"Knob1","Knob2","Knob3","Knob4","Knob5","Knob6"});
         }
      
}
static void bind_scene(){switch(level){
case 0:
Global.playAtmoLoop("Univerzal/Detektivka",1,4);
addQuadSensor(_layerScene,"BG",0,0,gameWidth,gameHeight);
addQuadSensor(_layerScene,"Domek",721,178,155,202);
break;
case 1:
Global.playAtmoLoop("Lyzar/AtmoLyzar",1,4);

         animCedulka = _layerScene.getChildByName("Cedulka");

         animSberac1 = _layerScene.getChildByName("Sberac1");

         animSberac2 = _layerScene.getChildByName("Sberac2");

         animSberac3 = _layerScene.getChildByName("Sberac3");

         animSberac4 = _layerScene.getChildByName("Sberac4");

         animSberac5 = _layerScene.getChildByName("Sberac5");

         animSberac6 = _layerScene.getChildByName("Sberac6");

         animSberac7 = _layerScene.getChildByName("Sberac7");

         animKurak = _layerScene.getChildByName("Kurak");

         animTabak = _layerScene.getChildByName("Tabak");

         animKour = _layerScene.getChildByName("Kour");

         animCudl = _layerScene.getChildByName("Cudl");

         animLyzar = _layerScene.getChildByName("Lyzar");
addQuadSensor(_layerScene,"BG",0,0,gameWidth,gameHeight);
addQuadSensor(_layerScene,"Cedulka",1043,180,65,150);
addQuadSensor(_layerScene,"Sberac1",472,508,65,110);
addQuadSensor(_layerScene,"Sberac3",956,571,91,110);
addQuadSensor(_layerScene,"Sberac2",308,313,67,86);
addQuadSensor(_layerScene,"Sberac6",340,129,67,86);
addQuadSensor(_layerScene,"Sberac4",1621,198,67,86);
addQuadSensor(_layerScene,"Sberac5",1742,425,67,86);
addQuadSensor(_layerScene,"Sberac7",1776,781,67,86);
addQuadSensor(_layerScene,"RukaDymka",806,973,110,182);
addQuadSensor(_layerScene,"Cudl",1389,830,82,74);
addQuadSensor(_layerScene,"Klic",1202,1299,198,119).rotation = -0.418879020478639;
addQuadSensor(_layerScene,"Kolo",1321,287,77,78);
addQuadSensor(_layerScene,"Lyzar",1067,252,108,89);
addQuadSensor(_layerScene,"Lano1",1218,243,64,217).rotation = -0.322761993242059;
addQuadSensor(_layerScene,"Lano2",1292,462,74,217).rotation = -1.31800632859454;

         disableSensors({"Klic","Lyzar"});
break;
case 2:
Global.playAtmoLoop("Rybar/AtmoRybar",1,4);

         animLyzar = _layerScene.getChildByName("Lyzar");

         animSup = _layerScene.getChildByName("Sup");

         animVcela = _layerScene.getChildByName("Vcela");

         animKoza1 = _layerScene.getChildByName("Koza1");

         animKoza2 = _layerScene.getChildByName("Koza2");

         animKoza3 = _layerScene.getChildByName("Koza3");

         animKostricka = _layerScene.getChildByName("Kostricka");

         animKour = _layerScene.getChildByName("Kour");

         animVoda = _layerScene.getChildByName("Voda");

         animRybar = _layerScene.getChildByName("Rybar");

         animMloci = _layerScene.getChildByName("Mloci");

         animMloci.stop();

         animMloci.setTouchable(true);
addQuadSensor(_layerScene,"BG",0,0,gameWidth,gameHeight);
addQuadSensor(_layerScene,"Koza1",738,231,108,89);
addQuadSensor(_layerScene,"Koza2",1706,603,108,89);
addQuadSensor(_layerScene,"Koza3",1220,689,108,89);
addQuadSensor(_layerScene,"Lyzar",1501,571,108,89);
addQuadSensor(_layerScene,"Splavek",216,1411,108,89);
addQuadSensor(_layerScene,"Sup",357,538,108,89);
addQuadSensor(_layerScene,"Dira",1256,976,108,89);
addQuadSensor(_layerScene,"Prut",260,600,90,67);

         disableSensors({"Sup","Lyzar"});
break;
case 3:
Global.playAtmoLoop("Vcely/AtmoVcely",1,4);

         animVcely = _layerScene.getChildByName("Vcely");

         animKvety = _layerScene.getChildByName("Kvety");

         animSosak = _layerScene.getChildByName("Sosak");

         animStrom = _layerScene.getChildByName("Strom");

         animSkret = _layerScene.getChildByName("Skret");

         animZarovka = _layerScene.getChildByName("Zarovka");

         animSvetlo = _layerScene.getChildByName("Svetlo");

         animGondola = _layerScene.getChildByName("Gondola");

         animVypinac = _layerScene.getChildByName("Vypinac");

         sprKrytkaSosakStrom = _layerScene.getChildByName("KrytkaSosakStrom");

         sprKrytkaSosakStrom.visible(false);
addQuadSensor(_layerScene,"BG",0,0,gameWidth,gameHeight);
addQuadSensor(_layerScene,"Kytka",1254,290,310,374).enabled = false;
addQuadSensor(_layerScene,"Dira",626,350,108,89);
addQuadSensor(_layerScene,"ZarovkaNasazena",578,50,72,89).enabled = false;
addQuadSensor(_layerScene,"Strom0",1538,926,153,133);
addQuadSensor(_layerScene,"Strom1",1487,904,165,166).enabled = false;
addQuadSensor(_layerScene,"Strom2",1430,892,165,166).enabled = false;
addQuadSensor(_layerScene,"Strom3",1377,889,165,166).enabled = false;
addQuadSensor(_layerScene,"Strom4",1312,887,165,166).enabled = false;
addQuadSensor(_layerScene,"DiraSosak",559,1114,108,119);
addQuadSensor(_layerScene,"Lebka",241,827,59,89);
addQuadSensor(_layerScene,"Vypinac",929,757,72,89);
addQuadSensor(_layerScene,"Skret",997,147,72,77).enabled = false;
addQuadSensor(_layerScene,"DoGondoly",805,131,144,145).enabled = false;
addQuadSensor(_layerScene,"Gondola",796,139,60,45).enabled = false;
break;
case 4:

         animDatel = _layerScene.getChildByName("Datel");

         animCerv1 = _layerScene.getChildByName("Cerv1");

         animCerv2 = _layerScene.getChildByName("Cerv2");

         animCerv3 = _layerScene.getChildByName("Cerv3");

         animCerv4 = _layerScene.getChildByName("Cerv4");

         animSovaPozice = _layerScene.getChildByName("SovaPozice");

         animGondola = _layerScene.getChildByName("Gondola");

         animLano = _layerScene.getChildByName("Lano");

         animVeverka = _layerScene.getChildByName("Veverka");

         animGramec = _layerScene.getChildByName("Gramec");

         animDutina = _layerScene.getChildByName("Dutina");

         animVetev = _layerScene.getChildByName("Vetev");
addQuadSensor(_layerScene,"BG",0,0,gameWidth,gameHeight);
addQuadSensor(_layerScene,"Gramec",984,841,154,176).enabled = false;
addQuadSensor(_layerScene,"Veverka",821,803,120,234).enabled = false;
addQuadSensor(_layerScene,"Sova0",453,186,132,196);
addQuadSensor(_layerScene,"Sova1",461,186,132,196).enabled = false;
addQuadSensor(_layerScene,"Sova2",637,186,132,196).enabled = false;
addQuadSensor(_layerScene,"Sova3",1200,339,132,196).enabled = false;
addQuadSensor(_layerScene,"Gondola",1395,598,246,239).enabled = false;
addQuadSensor(_layerScene,"Drzak",1470,548,85,50).enabled = false;
addQuadSensor(_layerScene,"Cerv1",645,48,85,90).enabled = false;
addQuadSensor(_layerScene,"Cerv2",1140,222,85,90).enabled = false;
addQuadSensor(_layerScene,"Cerv3",367,1266,85,90).enabled = false;
addQuadSensor(_layerScene,"Cerv4",1114,1104,85,90).enabled = false;
addQuadSensor(_layerScene,"Datel",167,410,120,108);
addQuadSensor(_layerScene,"Dutina",792,434,174,177);
break;
case 5:
Global.playAtmoLoop("Vodarna/AtmoVodarna",1,4);

         animRozvadec = _layerScene.getChildByName("Rozvadec");

         animMravec = _layerScene.getChildByName("Mravec");

         animMravenec = _layerScene.getChildByName("Mravenec");

         animDvere = _layerScene.getChildByName("Dvere");

         animSkret = _layerScene.getChildByName("Skret");

         animGondola = _layerScene.getChildByName("Gondola");

         animKlika = _layerScene.getChildByName("Klika");

         animHouba1 = _layerScene.getChildByName("Houba1");

         animHouba2 = _layerScene.getChildByName("Houba2");
addQuadSensor(_layerScene,"BG",0,0,gameWidth,gameHeight);
addQuadSensor(_layerScene,"Klika1",557,599,65,163).rotation = 0.523598775598299;
addQuadSensor(_layerScene,"Klika2",426,610,65,165).rotation = -0.465854557296066;
addQuadSensor(_layerScene,"Schody1",683,936,84,341).rotation = -0.697114173843819;
addQuadSensor(_layerScene,"Schody2",985,551,100,386).rotation = 1.04388666160556;
addQuadSensor(_layerScene,"Rozvadec",1035,480,82,82).enabled = false;
addQuadSensor(_layerScene,"Dvere",1133,455,86,149);
addQuadSensor(_layerScene,"Houba1",332,1335,126,82);
addQuadSensor(_layerScene,"Houba2",436,1463,92,63);
addQuadSensor(_layerScene,"Houba3",527,1364,82,65);
addQuadSensor(_layerScene,"Houba5",1348,918,62,57);
addQuadSensor(_layerScene,"Houba4",1313,1028,54,55);
addQuadSensor(_layerScene,"Houba6",1435,1044,40,51);
addQuadSensor(_layerScene,"Houba7",328,1494,60,82);
addQuadSensor(_layerScene,"Houba8",543,1464,33,68);
addQuadSensor(_layerScene,"Houba9",593,1445,33,68);
addQuadSensor(_layerScene,"Houba10",620,1485,33,57);
addQuadSensor(_layerScene,"Skret",567,1257,82,107).enabled = false;
addQuadSensor(_layerScene,"Knob1",1405,331,82,82).enabled = false;
addQuadSensor(_layerScene,"Knob2",1517,331,82,82).enabled = false;
addQuadSensor(_layerScene,"Knob3",1630,331,82,82).enabled = false;
addQuadSensor(_layerScene,"Knob4",1403,526,82,82).enabled = false;
addQuadSensor(_layerScene,"Knob5",1516,526,82,82).enabled = false;
addQuadSensor(_layerScene,"Knob6",1633,526,82,82).enabled = false;

         disableSensors({"Klika2","Schody1","Schody2","Houba1","Houba2"});
addQuadSensor(_layerScene,"Gondola",376,1105,82,173).enabled = false;
addQuadSensor(_layerScene,"GondolaTlac",376,1106,82,60).enabled = false;
break;
case 6:
Global.playAtmoLoop("Strojovna/AtmoStrojovna",1,0);

         animSvetlo = _layerScene.getChildByName("Svetlo");

         animKour0 = _layerScene.getChildByName("Kour0");

         animKour1 = _layerScene.getChildByName("Kour1");

         animKour2 = _layerScene.getChildByName("Kour2");

         animKour3 = _layerScene.getChildByName("Kour3");

         animRafika1 = _layerScene.getChildByName("Rafika1");

         animRafika2 = _layerScene.getChildByName("Rafika2");

         animSkret = _layerScene.getChildByName("Skret");

         animZebrik = _layerScene.getChildByName("Zebrik");

         animKolo = _layerScene.getChildByName("Kolo");

         animKoloStin = _layerScene.getChildByName("KoloStin");

         animDioda = _layerScene.getChildByName("Dioda");

         animVypinac = _layerScene.getChildByName("Vypinac");

         animPaka = _layerScene.getChildByName("Paka");
addQuadSensor(_layerScene,"BG",0,0,gameWidth,gameHeight);
addQuadSensor(_layerScene,"Plus",842,928,85,90);
addQuadSensor(_layerScene,"Minus",835,1072,85,90);
addQuadSensor(_layerScene,"Zebr",842,1018,88,57);
addQuadSensor(_layerScene,"Kour1",936,961,88,57);
addQuadSensor(_layerScene,"Kour2",936,1019,88,57);
addQuadSensor(_layerScene,"Kour3",938,1076,88,57);
addQuadSensor(_layerScene,"Vypinac",1714,969,76,68);
addQuadSensor(_layerScene,"Skret",1691,1085,88,131);
addQuadSensor(_layerScene,"Paka",1491,252,174,117);
addQuadSensor(_layerScene,"Zebrik",1518,357,97,845);
mGlobal.disableAllSensors();mGlobal.enableSensor("Vypinac");
break;
}}
