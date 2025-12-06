<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.11.2" name="roguelike" tilewidth="64" tileheight="64" tilecount="4" columns="0">
 <grid orientation="orthogonal" width="1" height="1"/>
 <tile id="0">
  <properties>
   <property name="door" type="bool" value="true"/>
   <property name="solid" type="bool" value="true"/>
  </properties>
  <image source="../designs/door.png" width="64" height="64"/>
 </tile>
 <tile id="1">
  <properties>
   <property name="door" type="bool" value="true"/>
   <property name="solid" type="bool" value="true"/>
  </properties>
  <image source="../designs/doors.png" width="64" height="64"/>
 </tile>
 <tile id="2">
  <properties>
   <property name="solid" type="bool" value="false"/>
  </properties>
  <image source="../designs/floor.png" width="64" height="64"/>
 </tile>
 <tile id="3">
  <properties>
   <property name="solid" type="bool" value="true"/>
  </properties>
  <image source="../designs/wall.png" width="64" height="64"/>
 </tile>
</tileset>
