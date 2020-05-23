$fn=50;

difference()
{
    cube([80,100,5],center=true);

    // LED Spaces
    union() {
        for (YPos=[-35:10:35])
            for (XPos=[-35:10:35]) 
                translate([XPos,YPos,9]) 
                    cube([8,8,20],center=true);
    }    

    // Cutout for PCB Resistors
    translate([0, 30,5]) cube([85,18,7],center=true);
    translate([0, 10,5]) cube([85,18,7],center=true);
    translate([0,-30,5]) cube([85,18,7],center=true);
    translate([0,-10,5]) cube([85,18,7],center=true);
    
    // Alignment-Holes
    translate([ 30,-45,0]) rotate([0,90,0]) cylinder(h=22,d1=3,d2=3,center=true);
    translate([ 30, 45,0]) rotate([0,90,0]) cylinder(h=22,d1=3,d2=3,center=true);
    translate([-30,-45,0]) rotate([0,90,0]) cylinder(h=22,d1=3,d2=3,center=true);
    translate([-30, 45,0]) rotate([0,90,0]) cylinder(h=22,d1=3,d2=3,center=true);

    // Mounting Holes
    translate([0, 45,0]) cylinder(h=20,d=5,center=true);
    translate([0,-45,0]) cylinder(h=20,d=5,center=true);
    translate([0,-45,-1]) cylinder(h=5,d1=10,d2=0,center=true);
    translate([0, 45,-1]) cylinder(h=5,d1=10,d2=0,center=true);

}

