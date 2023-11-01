<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.2" name="characters" tilewidth="32" tileheight="32" tilecount="80" columns="8">
 <image source="../textures/sprite_sheet_32x32.png" width="256" height="320"/>
 <tile id="8">
  <animation>
   <frame tileid="8" duration="300"/>
   <frame tileid="9" duration="300"/>
  </animation>
 </tile>
 <tile id="16">
  <animation>
   <frame tileid="16" duration="300"/>
   <frame tileid="17" duration="300"/>
  </animation>
 </tile>
 <tile id="24">
  <animation>
   <frame tileid="24" duration="300"/>
   <frame tileid="25" duration="300"/>
  </animation>
 </tile>
 <tile id="32">
  <animation>
   <frame tileid="32" duration="300"/>
   <frame tileid="33" duration="300"/>
  </animation>
 </tile> 
 <tile id="48">
  <animation>
   <frame tileid="48" duration="300"/>
   <frame tileid="49" duration="300"/>
  </animation>
 </tile>
 <tile id="56">
  <animation>
   <frame tileid="56" duration="300"/>
   <frame tileid="57" duration="300"/>
  </animation>
 </tile>
 <tile id="64">
  <animation>
   <frame tileid="64" duration="300"/>
   <frame tileid="65" duration="300"/>
  </animation>
 </tile>
 <tile id="72">
  <animation>
   <frame tileid="72" duration="300"/>
   <frame tileid="73" duration="300"/>
  </animation>
 </tile>
 <wangsets>
  <wangset name="man" type="corner" tile="2">
   <wangcolor name="up_standing" color="#ff0000" tile="0" probability="1"/>
   <wangcolor name="up_walking" color="#00ff00" tile="8" probability="1"/>
   <wangcolor name="right_standing" color="#0000ff" tile="1" probability="1"/>
   <wangcolor name="down_standing" color="#ff7700" tile="2" probability="1"/>
   <wangcolor name="left_standing" color="#00e9ff" tile="3" probability="1"/>
   <wangcolor name="right_walking" color="#ff00d8" tile="16" probability="1"/>
   <wangcolor name="down_walking" color="#ffff00" tile="24" probability="1"/>
   <wangcolor name="left_walking" color="#a000ff" tile="32" probability="1"/>
   <wangtile tileid="0" wangid="0,1,0,1,0,1,0,1"/>
   <wangtile tileid="1" wangid="0,3,0,3,0,3,0,3"/>
   <wangtile tileid="2" wangid="0,4,0,4,0,4,0,4"/>
   <wangtile tileid="3" wangid="0,5,0,5,0,5,0,5"/>
   <wangtile tileid="8" wangid="0,2,0,2,0,2,0,2"/>
   <wangtile tileid="9" wangid="0,2,0,2,0,2,0,2"/>
   <wangtile tileid="16" wangid="0,6,0,6,0,6,0,6"/>
   <wangtile tileid="17" wangid="0,6,0,6,0,6,0,6"/>
   <wangtile tileid="24" wangid="0,7,0,7,0,7,0,7"/>
   <wangtile tileid="25" wangid="0,7,0,7,0,7,0,7"/>
   <wangtile tileid="32" wangid="0,8,0,8,0,8,0,8"/>
   <wangtile tileid="33" wangid="0,8,0,8,0,8,0,8"/>
  </wangset>
  <wangset name="soldier" type="corner" tile="42">
   <wangcolor name="up_standing" color="#ff0000" tile="40" probability="1"/>
   <wangcolor name="up_walking" color="#00ff00" tile="48" probability="1"/>
   <wangcolor name="right_standing" color="#0000ff" tile="41" probability="1"/>
   <wangcolor name="down_standing" color="#ff7700" tile="42" probability="1"/>
   <wangcolor name="left_standing" color="#00e9ff" tile="43" probability="1"/>
   <wangcolor name="right_walking" color="#ff00d8" tile="56" probability="1"/>
   <wangcolor name="down_walking" color="#ffff00" tile="64" probability="1"/>
   <wangcolor name="left_walking" color="#a000ff" tile="72" probability="1"/>
   <wangtile tileid="40" wangid="0,1,0,1,0,1,0,1"/>
   <wangtile tileid="41" wangid="0,3,0,3,0,3,0,3"/>
   <wangtile tileid="42" wangid="0,4,0,4,0,4,0,4"/>
   <wangtile tileid="43" wangid="0,5,0,5,0,5,0,5"/>
   <wangtile tileid="48" wangid="0,2,0,2,0,2,0,2"/>
   <wangtile tileid="49" wangid="0,2,0,2,0,2,0,2"/>
   <wangtile tileid="56" wangid="0,6,0,6,0,6,0,6"/>
   <wangtile tileid="57" wangid="0,6,0,6,0,6,0,6"/>
   <wangtile tileid="64" wangid="0,7,0,7,0,7,0,7"/>
   <wangtile tileid="65" wangid="0,7,0,7,0,7,0,7"/>
   <wangtile tileid="72" wangid="0,8,0,8,0,8,0,8"/>
   <wangtile tileid="73" wangid="0,8,0,8,0,8,0,8"/>
  </wangset>
 </wangsets>
</tileset>
