## LZMD : Lempel-Ziv Montecarlo Diffusion

### Update:
I got rejected from YCombinator last week, ZFellows in August and got ghosted by VCs inbetween. 
/
I took a gap year to build this and burnt through my savings. Rejection is depressing lol.
/
Please reach out - murage@fileforma.com - if you want to angel/lead my pre-seed round. 
Here's my [pitch deck](https://docs.google.com/presentation/d/1jzcLHrZaQfi0oOKk9JtY_K_IfVrcVjTeQgZzMfiYNm4/edit)
/
I aspire to get terms similar to YCombinator.
### Disclaimer
Data compression is a solved problem. No ✨ magical algorithm ✨ makes files infinitely small. Also, you can't compress random data.

### How It Works
We built a custom diffusion model to *simulate the weird change in volume when water and alcohol mix*.

It's a navier-stokes solver whose solutions lead to smaller file sizes. This is **NOT** a data compressor. We don't find probabilites.  
/

### Technical Overview

A mixture of equal parts water and ethanol presents a [volume 10% less](https://www.flinnsci.com/api/library/Download/bc2d91b9d50f4ead80c9ff320b9a4419) than the sum of individual volumes.
Ethanol molecules are smaller than water molecules. When mixed, small ethanol molecules [occupy the spaces between ](https://www.thoughtco.com/miscibility-of-fluids-608180) large water molecules. 

/
We built a mathematical model to simulate this mizture. We :
- Split a compressed file into two parts - Left and Right analogous to Water and Ethanol.
- Find large empty spaces in the Left part.
- Find small integers in the Right part.
- Place the small integers inside the large spaces.
