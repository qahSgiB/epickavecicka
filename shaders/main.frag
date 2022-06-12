#version 450

layout(location = 0) in vec3 fs_position;
layout(location = 1) in vec3 fs_normal;

layout(location = 0) out vec4 final_color;

void main() {
    vec3 color = vec3((fs_position.x + 2.5) / 7, (fs_position.y + 0.7) / 1.4, (fs_position.z + 5.0) / 10);
    // vec3 color = vec3(0.0, 0.0, 0.0);
    // color = floor(color * 7.0f) / 7.0f;
    final_color = vec4(color, 1.0);
}
