#version 330 core
out vec3 FragColor;
uniform vec3 color;
void main()
{
   FragColor = vec3(color.x,color.y,color.z);
}
