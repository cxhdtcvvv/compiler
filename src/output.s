str	fp, [sp, #-4]!
add	fp, sp, #0
sub	sp, sp, #28
mov r3, 0
str r3, [fp, #-8]
mov r3, 0
str r3, [fp, #-12]
mov r3, 0
str r3, [fp, #-16]
mov r3, 0
str r3, [fp, #-20]
mov r3, 0
str r3, [fp, #-24]
mov r0, #9
mov r1, #5
add r1, r0, r1
mov r3, r1
mov r0, #3
mov r1, #4
mul r1, r0, r1
add r1, r3, r1
mov r3, r1
str r3, [fp, #-28]
mov r1, #1
mov r3, r1
str r3, [fp, #-8]
mov r1, #88
mov r3, r1
str r3, [fp, #-12]
ldm r0, [fp, #-8]
ldm r1, [fp, #-12]
mul r1, r0, r1
mov r0, #1
add r1, r0, r1
mov r3, r1
str r3, [fp, #-16]
mov r1, #80
mov r3, r1
str r3, [fp, #-8]