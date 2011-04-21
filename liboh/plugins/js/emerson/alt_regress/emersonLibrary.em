
util = {    };

util.Vec3 = function (x,y,z)
{
    this.x = x;
    this.y = y;
    this.z = z;
    this.print = function()
    {
        print("\n\nPrinting\n\n");
        print(this.x);
        print(this.y);
        print(this.z);
        print("\n\n");
    };
};

util.minus = function (a,b)
{
    //super primitive checking to see if a is a vector
    if (typeof(a) == 'object')
    {
        //comment out next line and uncomment out line after when running through emerson.  Uncomment next line and uncomment next if running rhino
        return new util.Vec3(a.x-b.x, a.y-b.y, a.z-b.z);            
        //return <a.x-b.x, a.y-b.y, a.z-b.z>;            
    }

    return a-b;
};


util.plus = function(a,b)
{
    //super primitive checking to see if a is a vector
    if (typeof(a) == 'object')
    {
        //comment out next line and uncomment out line after when running through emerson.  Uncomment next line and uncomment next if running rhino
        return new util.Vec3(a.x+b.x,  a.y+b.y,a.z+b.z);            
        //return <a.x+b.x,  a.y+b.y,a.z+b.z>;            
    }

    return a+b;
};

util.Pattern = function (a, b, c)
{
    print("\nprinting pattern\n");
    print(a);
    print(b);
    print(c);
    this.a = a;
    this.b = b;
    this.c = c;
    

    this.print = function()
    {
        print("\nprinting pattern2\n");
        print(this.a);
        print(this.b);
        print(this.c);
    };
};


system = {    };

system.incrementer = 0;

system.registerHandler = function (a, b, c, d)
{
    ++this.incrementer;

    print("\nRegistered handler\n");
    print(system.incrementer);
    print(a);
    print(b);
    print(c);
    print(d);
    print("\n\nDone registering handlers\n\n");

    if (c instanceof Array)
    {
        for (var s=0; s < c.length; ++s)
            c[s].print();
    }
        
};
