#version 330 core

uniform sampler2D uColorSampler;

varying vec2 vTexcoord;

void main() {
    gl_FragColor = texture(uColorSampler, vTexcoord);
}
