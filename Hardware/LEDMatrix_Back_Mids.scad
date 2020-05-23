$fn=50;

difference()
{
    cube([80,100,5],center=true);

    
    // Alignment-Holes
    translate([ 30,-45,0]) rotate([0,90,0]) cylinder(h=22,d1=3,d2=3,center=true);
    translate([ 30, 45,0]) rotate([0,90,0]) cylinder(h=22,d1=3,d2=3,center=true);
    translate([-30,-45,0]) rotate([0,90,0]) cylinder(h=22,d1=3,d2=3,center=true);
    translate([-30, 45,0]) rotate([0,90,0]) cylinder(h=22,d1=3,d2=3,center=true);

    // Mounting Holes
    translate([0, 45,0]) cylinder(h=20,d=5,center=true);
    translate([0,-45,0]) cylinder(h=20,d=5,center=true);

    // Holes
    cube([30,60,20],center=true);
    translate([ 40,0,0]) cube([30,60,20],center=true);
    translate([-40,0,0]) cube([30,60,20],center=true);
}


