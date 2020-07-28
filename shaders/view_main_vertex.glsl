#version 330 core
attribute vec3 aPosition;
attribute vec2 aTexcoord;

uniform mat4 uMMatrix;
uniform mat4 uVMatrix;
uniform mat4 uPMatrix;

varying vec2 vTexcoord;

void main()
{
    gl_Position = uPMatrix * uVMatrix * uMMatrix * vec4(aPosition, 1.0);

    vTexcoord = aTexcoord;
}
