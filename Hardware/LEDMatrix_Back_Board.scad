$fn=50;

difference()
{
    cube([100,100,5],center=true);

    // Mounting Holes
    translate([ 40, 45,0]) cylinder(h=20,d=3.5,center=true);
    translate([ 40,-45,0]) cylinder(h=20,d=3.5,center=true);
    translate([-40, 45,0]) cylinder(h=20,d=3.5,center=true);
    translate([-40,-45,0]) cylinder(h=20,d=3.5,center=true);

    translate([ 40,-45,-1]) cylinder(h=5,d1=7,d2=0,center=true);
    translate([-40,-45,-1]) cylinder(h=5,d1=7,d2=0,center=true);
    translate([ 40, 45,-1]) cylinder(h=5,d1=7,d2=0,center=true);
    translate([-40, 45,-1]) cylinder(h=5,d1=7,d2=0,center=true);

    // Holes
//    cube([60,60,20],center=true);
//    translate([ 40,0,0]) cube([30,60,20],center=true);
//    translate([-40,0,0]) cube([30,60,20],center=true);

    // Arduino-Holes
    
    translate([-55,-24,0]) union() {
        translate([13.97, 2.54,0]) cylinder(h=20,d=3.5,center=true);           // Bottom Left
        translate([15.24,50.8, 0]) cylinder(h=20,d=3.5,center=true);           // Top Left
        translate([96.52, 2.54,0]) cylinder(h=20,d=3.5,center=true);           // Bottom Right
        translate([90.17,50.8 ,0]) cylinder(h=20,d=3.5,center=true);           // Top Right
    }
    cube([50,60,20],center=true);

}

