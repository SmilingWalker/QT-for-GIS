attribute vec2 attrPos;
uniform mat4 modelview;
uniform mat4 projection;
void main()
{
   gl_Position = projection * modelview *vec4(attrPos.x,attrPos.y,0,1.0);
}
