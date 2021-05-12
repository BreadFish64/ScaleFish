# ScaleFish
A project researching texture upscaling methods.

## ScaleForce
So far the only successfull algorithm created is "ScaleForce". ScaleForce calculates the difference between a sample and the surrounding samples in YUV with alpha correction. It then finds the vector sum of these difference and takes another sample, subtracting the sum from the original sample position.

Imagine an single channel image

Original sample and surrounding values
||L|C|R|
|-|-|-|-|
|T|0.5|0.75|0.75|
|C|0.5|0.5|0.75|
|B|0.5|0.75|0.5|

Differences from original sample
||L|C|R|
|-|-|-|-|
|T|0|0.25|0.25|
|C|0|0|0.25|
|B|0|0.25|0|

The vector sum is `<0.5, 0.25>`. The new sample should then be bilinearly filtered at `position - sum`.
In this case the new value should be  
`(0.375 * CL) + (0.375 * CC) + (0.125 * BL) + (0.125 * BC) = 0.531`.  
By contrast, the average of all 9 original samples is `0.611`.

There is addition clamping logic to prevent the sample location from moving too far and introducing artifacts.  
A live WebGL demo can be found here: https://breadfish64.github.io/scaleforce/
