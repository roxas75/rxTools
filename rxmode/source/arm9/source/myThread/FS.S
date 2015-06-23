.text

.thumb
.global fopen9
.type fopen9, %function
	fopen9:
		push {r0-r6, lr}
		ldr r4, =0x0805B181
		blx r4
		pop {r0-r6, pc}
.pool

.thumb
.global fwrite9
.type fwrite9, %function
	fwrite9:
		push {r4, lr}
		ldr r4, =0x0805C4D1
		blx r4
		pop {r4, pc}
.pool

.thumb
.global fread9
.type fread9, %function
	fread9:
		push {r4, lr}
		ldr r4, =0x0804D9B1
		blx r4
		pop {r4, pc}
.pool

.thumb
.global fclose9
.type fclose9, %function
	fclose9:
		push {r4, lr}
		ldr r4, =0x0805B26D
		blx r4
		pop {r4, pc}
.pool
