$input a_position
$output v_nearPoint, v_farPoint

#include <bgfx_shader.sh>

vec3 UnprojectPoint(float x, float y, float z) {
    vec4 unprojectedPoint =  mul(u_invView, mul(u_invProj, vec4(x, y, z, 1.0)));
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
	v_farPoint = UnprojectPoint(a_position.x, a_position.y, 1.0).xyz; // unprojecting on the near plane
    v_nearPoint = UnprojectPoint(a_position.x, a_position.y, 0.0).xyz; // unprojecting on the far plane

	gl_Position = vec4(a_position, 1.0); //mul(u_modelViewProj, vec4(a_position,1.0));
}