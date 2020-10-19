# TP2SO2020

_Construcción del Núcleo de un Sistema Operativo y estructuras de administración de recursos._

## Instrucciones de instalacion

Environment setup:
1- Install the following packages before building the Toolchain and Kernel:

```
nasm qemu gcc make
```

2- Build the Toolchain

Execute the following commands on the x64BareBones project directory:

```
user@linux:$ cd Toolchain
user@linux:$ make all
```

3- Build the Kernel

From the x64BareBones project directory run:

```
user@linux:\$ make all IMP=BUDDY
```

to compile with the BUDDY memory manager
or run

```
user@linux:\$ make all IMP=BITMAP
```

to compile with the BITMAP memory manager

4- Run the kernel

From the x64BareBones project directory run:

```
user@linux:\$ ./run.sh
```

## Grupo 13

- **Lautaro Galende**
- **Valentín Ratti**
- **Tommy Rosenblatt**
- **Sebastián Itokazu**
