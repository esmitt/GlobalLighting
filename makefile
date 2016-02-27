CC = g++
LDFLAGS = -lglfw3 -lGLEW -lGLU -lGL -lX11 -lXxf86vm -lXrandr -lpthread -lXi -lfreeimage -lassimp 
#-l/usr/lib64/libGLEW.so.1.10
CFLAGS = -lglfw3 -lGLEW -lGLU -lGL -lX11 -lXxf86vm -lXrandr -lpthread -lXi -lfreeimage -lassimp

all: prog

prog: main
	$(CC) *.o -o main $(CFLAGS) $(LDFLAGS)

main: texture boundingbox 3dmodel camera fullscreen glslprog gbuffer application main.cpp
	$(CC) main.cpp -c $(CFLAGS)

application: Application.h Application.cpp
	$(CC) Application.cpp -c $(CFLAGS)

gbuffer: GBuffer.h GBuffer.cpp
	$(CC) GBuffer.cpp -c $(CFLAGS)
	
glslprog: GLSLProgram.h GLSLProgram.cpp
	$(CC) GLSLProgram.cpp -c $(CFLAGS)
	
fullscreen: FullScreenQuad.h FullScreenQuad.cpp
	$(CC) FullScreenQuad.cpp -c $(CFLAGS)
	
camera: Camera.h Camera.cpp
	$(CC) Camera.cpp -c $(CFLAGS)
	
3dmodel: loader/3DModel.h loader/3DModel.cpp
	$(CC) loader/3DModel.cpp -c $(CFLAGS)
	
boundingbox: loader/BoundingBox.h loader/BoundingBox.cpp
	$(CC) loader/BoundingBox.cpp -c $(CFLAGS)
	
texture: loader/Texture.cpp loader/Texture.cpp
	$(CC) loader/Texture.cpp -c $(CFLAGS)
	
.PHONY: clean

clean: 
	rm *.o main -f

# g++ *.cpp -lglfw3 -lGLEW -lGL -lGLU -lX11 -lXxf86vm -lXran^C -lpthread -lXi -lfreeimage -lassimp
#g++ main.cpp -o main.exec loader/*.o *.o -lglfw3 -lGLEW -lGL -lGLU -lX11 -lXxf86vm -lXrandr -lpthread -lXi -lfreeimage -lassimp -L/usr/lib64/
