
<?PHP include("insert_commands.php"); ?>
</head>
<body>
<form name="myform">
   <center><b>Action:</b> <input type="text" name="stage" size="7" /><br>
       <b>Param:</b>   <input type="text" name="param" size="3" value="0"/>
       <b>Speed:</b>  <input type="text" id="speed" size="3" value="0-255"  onchange="changeSpeed()"/></center>
</form>
<!-- Create  Mappings -->
<img src="images/button_box.png" alt="Navigational Buttons" 
        border="0" usemap="#buttons"/>

<map name="buttons">

   <area shape="circle" 
            coords="43,43,32"
             
            onClick="sendCommand(3,0)"
            onMouseOver="showButtons('Look Left')" 
            onMouseOut="showButtons('')"/>
 
   <area shape="circle" 
            coords="130,43,32"
            onClick="sendCommand(5,0)"
            onMouseOver="showButtons('Look Ahead')" 
            onMouseOut="showButtons('')"/>
 
   <area shape="circle" 
            coords="217,43,32"
            onClick="sendCommand(4,0)"
            onMouseOver="showButtons('Look Right')" 
            onMouseOut="showButtons('')"/>


   <area shape="circle" 
            coords="43,130,32"
            onClick="sendCommand(20,0)"
            onMouseOver="showButtons('Show Reports')" 
            onMouseOut="showButtons('')"/>
 
   <area shape="circle" 
            coords="130,130,32"
            onClick="sendCommand(10,500)" 
            onMouseOver="showButtons('Move Forward')" 
            onMouseOut="showButtons('')"/>
 
   <area shape="circle" 
            coords="217,130,32"
            onClick="sendCommand(90,0)"
            onMouseOver="showButtons('Take Picture')" 
            onMouseOut="showButtons('')"/>

   <area shape="circle" 
            coords="43,217,32"
            onClick="sendCommand(12,500)"
            onMouseOver="showButtons('Turn Left')" 
            onMouseOut="showButtons('')"/>
 
   <area shape="circle" 
            coords="130,217,32"
            onClick="sendCommand(0,0)"
            onMouseOver="showButtons('Full Stop')" 
            onMouseOut="showButtons('')"/>
 
   <area shape="circle" 
            coords="217,217,32"
            onClick="sendCommand(13,500)"
            onMouseOver="showButtons('Turn Right')" 
            onMouseOut="showButtons('')"/>


   <area shape="circle" 
            coords="43,304,32"
            onClick="sendCommand(2,1)"
            onMouseOver="showButtons('Sonar Sweep')" 
            onMouseOut="showButtons('')"/>
 
   <area shape="circle" 
            coords="130,304,32"
            onClick="sendCommand(11,500)"
            onMouseOver="showButtons('Move Backward')" 
            onMouseOut="showButtons('')"/>
 
   <area shape="circle" 
            coords="217,304,32"
            onClick="sendCommand(80,0)"
            onMouseOver="showButtons('Settings')" 
            onMouseOut="showButtons('')"/>

</map>

	

