<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.11.2" name="image" tilewidth="4" tileheight="4" tilecount="16" columns="4">
 <image source="image.png" width="18" height="18"/>
 <tile id="0">
  <animation>
   <frame tileid="0" duration="10"/>
   <frame tileid="1" duration="20"/>
  </animation>
 </tile>
 <tile id="1">
  <properties>
   <property name="BoolProp" type="bool" value="true"/>
   <property name="ColourProp" type="color" value="#ff00ff00"/>
   <property name="FloatProp" type="float" value="3.14"/>
   <property name="IntProp" type="int" value="42"/>
   <property name="StringProp" value="Hello World"/>
  </properties>
 </tile>
</tileset>
