import math

# sine wavetable, 1 cycle over 1024 points
f = open('sineLUT.txt', 'w')
f.write("const float lut_sine[1025] = {")
for i in range(1024):   
    f.write(str(round(math.sin(2*math.pi*i/1024), 6))+", ")

f.write("0")
f.write("};")
f.close()

# MIDI note [0, 127] to frequency (Hz)
f = open('mtof.txt', 'w')
f.write("const float mtof[128] = {")
for i in range(127):   
    f.write(str(round(440*math.pow(2,(i-69)/12), 6))+", ")
f.write(str(round(440*math.pow(2,(127-69)/12), 6)))
f.write("};")
f.close()


# Pitch bend value to frequency multiplier
f = open('pbend1024.txt', 'w')
f.write("const float pbend[1024] = {")
# negative bend
for i in range(512):
    i = i/512 - 1
    f.write(str(round(math.pow(2,i), 6))+", ")

# positive bend
for i in range(511):
    i = i / 511
    f.write(str(round(math.pow(2,i), 6))+", ")
f.write(str(round(math.pow(2, 1), 6)))

f.write("};")
f.close()


# Log table for exponential ADSR
f = open('log_lut.txt', 'w')
f.write("const float log_lut[1025] = {")
# negative bend
for i in range(1024):
    i = 0.0001 + i/1024
    f.write(str(round(math.log(i), 6))+", ")

f.write("0")
f.write("};")
f.close()

# Tan table for filter coefficients
f = open('tan_lut.txt', 'w')
f.write("const float tan_lut[2049] = {")
# negative bend
for i in range(2048):
    i = i*1.570797/2048
    f.write(str(round(math.tan(i), 6))+", ")

f.write("0")
f.write("};")
f.close()