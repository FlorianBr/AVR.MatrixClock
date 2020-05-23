$fn=50;

difference()
{
    union() {
        cube([80,10,5],center=true);
        translate([ 40, 0,0]) cylinder(h=5,d=10,center=true);
        translate([-40, 0,0]) cylinder(h=5,d=10,center=true);
    }

    // Mounting Holes
    translate([ 40, 0,0]) cylinder(h=20,d=3.5,center=true);
    translate([-40, 0,0]) cylinder(h=20,d=3.5,center=true);
}

