# StallRunLib
This library was designed to run on Arduino compatible architectures. It was designed with [Visual Micro](https://www.visualmicro.com/) but is compatible with [Arduino IDE](https://www.arduino.cc/en/software).

## Description
The goal of this library is to simplify execution of code during down times like stalling loops.
So instead of making a loop like so:
```c++
while (!Serial.available()); // Or delay() if you want to reduce stress on the processor.
```
You would add a call to ```StallRun()``` like so:
```c++
while (!Serial.available()) StallRun(); // Runs code in the background.
```
This way, you don't get any stalling loops.


## Usage
### Setup
To use this library in your code, you will need to implement two methods: 
```c++ 
// Implement this function to run code during a stall time. (protected from reentry)
void InStall();
// Implement this function to run code without reentry prevention.
void ForcedRunStall();
```
The code in ```InStall()``` is protected from reentry. Consider the following example:
```c++
// Imagine this is a time consuming task
void Reenter()
{
	Serial.println("Reenter()");

	// Create reentry
	StallRun();
	
	// For the purpose of the example, let's leave it at that, but be aware that you could have multiple calls
	// to StallRun() without any issues.
}


void InStall()
{
	Serial.println("InStall()");

	// Reenter() is a task that needs to be executed often
	Reenter();

	// But since Reenter() calls StallRun() as a sub-function of InStall(), the reentry will be bypassed.
	// So after the first time Reenter() is called, it will no longer call InStall() to prevent stack overflow.
	// Now although InStall() does not run, ForcedRunStall() is still going to run at every StallRun() call.
}

// Will run regardless of reentry
void ForcedRunStall()
{
	Serial.println("ForcedRunStall()");
}

void setup()
{
	Serial.begin(115200);
	while (!Serial);
	
	// Calls ForcedRunStall() followed by InStall()
	StallRun();
}

void loop() { }
```
The code above will give you the following output:
```
ForcedRunStall()
InStall()
Reenter()
ForcedRunStall()
InStall()
Reenter()
ForcedRunStall()
```
Note that after the second ```Reenter()```, ```InStall()``` is not called. That is the reentry protection.
So ```ForcedRunStall()``` is always called first, ```InStall()``` follows, ```Reenter()``` is called and starts the whole process again until ```Reenter()``` is called for the second time. The call to ```StallRun()``` is flagged as reentry. So ```ForcedRunStall()``` is called, but ```InStall()``` is skipped completely to avoid reentry.

### StallInfo methods
There are 3 ```StallInfo``` methods that can be useful (the others are caled using macros):
 - [IsInitialized](#isinitialized)
 - [InitDone](#initdone)
 - [IsLowPriority](#islowpriority)

#### IsInitialized
```StallInfo.IsInitialized()```
Returns a boolean corresponding to whether or not ```StallInfo.InitDone()``` has been called.

#### InitDone
```StallInfo.InitDone()```
Call once the initialization of any dependencies of ```InStall()``` and ```ForcedRunStall()```

Example:
```c++
void setup()
{
    // Initialization
    Serial.begin(115200);
    while (!Serial); // If you need to run something else here, you could safely put StallRun() here
    
    // Notify end of initialization
    StallInfo.InitDone();
}

void loop()
{
    StallRun();
}

void ForcedRunStall() { }

void InStall()
{
    // Code here will run even before initialization is complete
    
    if (!StallInfo.IsInitialized()) return; // Anything past this point will only run once initialized
    Serial.println("Hi");
}
```

#### IsLowPriority
```StallInfo.IsLowPriority()```
Returns true if this is a low priority run. For more info see [StallRunLowPriority](#stallrunlowpriority).

### Stalling macros
There are 3 stalling macros:
 - [StallRun](#stallrun)
 - [StallRunLowPriority](#stallrunlowpriority)
 - [StallDelay](#stalldelay)

#### StallRun
```StallRun()```
Takes no argument. As discussed [here](#setup), this will call ```ForcedRunStall()``` and ```InStall()``` although ```InStall()``` is reentry protected.

#### StallRunLowPriority
```StallRunLowPriority(ms)```
| Parameter | Description |
| - | - |
| ms | Number of milliseconds between runs |

This macro will only call the implemented functions at ```ms``` milliseconds apart.

Example:
```c++
void InStall()
{
    // High priority code will run at the interval specified
    // Put fast executing code here
    Serial.print("Hi at: ");
    Serial.println(millis());
    
    
    if (StallInfo.IsLowPriority()) return; // or set your own rule for what happens in low priority situations.
    // Low priority code will not run
    // Put time consuming code here
}

void ForcedRunStall() { } // Suffers the same constraint as InStall()

void setup()
{
    Serial.begin(115200);
    while (!Serial);
}

void loop()
{
    StallRunLowPriority(200);
}
```
This code will output the following:
```
Hi at: 200
Hi at: 400
Hi at: 600
Hi at: 800
Hi at: 1000
Hi at: 1200
Hi at: 1400
Hi at: 1600
Hi at: 1800
...
```
Although the output was constantly 200 ms increments, it is not a guarantee that the code will always be called every 200ms. If there comes a moment where the time between two calls is higher than 200ms, the first call after over 200ms will be instant, and the following will keep their synchronicity.

#### StallDelay(ms)
```StallDelay(ms)```
| Parameter | Description |
| - | - |
| ms | Number of milliseconds to wait for |

This macro will safely wait for the correct amount of ms while calling StallRun(). This is a safe replacement for ```delay(ms)```

## Template
```c++
void setup()
{
    Serial.begin(115200);
    while (!Serial) StallRun();
    

    StallInfo.InitDone(); // Put this at the end of initialization phase of your code
}

void loop()
{
    StallRun(); // You most likely want to call StallRun() often
}

// Put any code that might cause StallRun() reentry
void InStall()
{
    // Initialization independant code here
    if (!StallInfo.IsInitialized()) return; // Initialization dependant code goes below this
    
    // High priority code here (preferably not time consuming)
    
    if (StallInfo.IsLowPriority()) return; // Low priority code goes below this
    // Put time consuming or less time critical code here
}

// Only put code here that is guaranteed not to have StallRun() reentry
void ForcedRunStall()
{
    // Initialization independant code here
    if (!StallInfo.IsInitialized()) return; // Initialization dependant code goes below this
    
    // High priority code here (preferably not time consuming)
    
    if (StallInfo.IsLowPriority()) return; // Low priority code goes below this
    // Put time consuming or less time critical code here
}
```
