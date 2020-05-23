$fn=50;

difference()
{
    union() {
        cube([100,100,5],center=true);
        translate([-50+2.5,0,12.5+2.5]) cube([5,70,25],center=true);
        translate([+50-2.5,0,12.5+2.5]) cube([5,70,25],center=true);
    }

    // Mounting Holes
    translate([ 40, 45,0]) cylinder(h=20,d=3.5,center=true);
    translate([ 40,-45,0]) cylinder(h=20,d=3.5,center=true);
    translate([-40, 45,0]) cylinder(h=20,d=3.5,center=true);
    translate([-40,-45,0]) cylinder(h=20,d=3.5,center=true);

    translate([ 40,-45,-1]) cylinder(h=5,d1=7,d2=0,center=true);
    translate([-40,-45,-1]) cylinder(h=5,d1=7,d2=0,center=true);
    translate([ 40, 45,-1]) cylinder(h=5,d1=7,d2=0,center=true);
    translate([-40, 45,-1]) cylinder(h=5,d1=7,d2=0,center=true);

    // Power connector
    translate([-50,0,15]) rotate([0,90,0]) union() {
        cylinder(h=20,d=11,center=true);
        cylinder(h=15,d1=0,d2=14,center=true);
        cylinder(h=15,d1=24,d2=0,center=true);
    }

    // Arduino-Holes (Nano)
    translate([-10,-20,0]) rotate([0,0,90]) union() {
        translate([      1.27, 1.27         ,0]) cylinder(h=20,d=2.5,center=true);      // Bottom Left
        translate([      1.27, 1.27+15.24   ,0]) cylinder(h=20,d=2.5,center=true);      // Top Left
        translate([43.18-1.27, 1.27         ,0]) cylinder(h=20,d=2.5,center=true);      // Bottom Right
        translate([43.18-1.27, 1.27+15.24   ,0]) cylinder(h=20,d=2.5,center=true);      // Top Right
        translate([      1.27, 1.27         ,-3]) cylinder(h=5,d1=10,d2=0,center=true);
        translate([      1.27, 1.27+15.24   ,-3]) cylinder(h=5,d1=10,d2=0,center=true);
        translate([43.18-1.27, 1.27         ,-3]) cylinder(h=5,d1=10,d2=0,center=true);
        translate([43.18-1.27, 1.27+15.24   ,-3]) cylinder(h=5,d1=10,d2=0,center=true);
    }

    translate([20,-35,0]) cylinder(h=20, d=3.5, center=true);   // Mounting Hole DHT
    translate([20,35,0]) cylinder(h=20, d=2.5, center=true);    // Mounting Hole RTC
}
