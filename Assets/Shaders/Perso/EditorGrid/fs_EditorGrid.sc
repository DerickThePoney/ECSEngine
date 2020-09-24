$input v_nearPoint, v_farPoint

#include <bgfx_shader.sh>

vec4 u_nearFar;

vec4 grid(vec3 fragPos3D, float scale, float baseGrey, float baseAxisMult) {
    vec2 coord = fragPos3D.xz / scale; // use the scale variable to set the distance between the lines
    vec2 derivative = fwidth(coord);
    
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float l = min(grid.x, grid.y);
    vec4 color = vec4(baseGrey, baseGrey, baseGrey, 1.0 - min(l, 1.0));

    float minimumz = min(1/derivative.y, 1);// * baseAxisMult;
    float minimumx = min(1/derivative.x, 1);// * baseAxisMult;

    // z axis
    if(fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
    {
    	color.z = 1.0;
    }
    // x axis
    if(fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
    {
    	color.x = 1.0;
    }
    return color;
}
float computeDepth(vec3 pos) {
    vec4 clip_space_pos = mul(u_proj, mul(u_view, vec4(pos.xyz, 1.0)));
    return (clip_space_pos.z / clip_space_pos.w);
}

float computeLinearDepth(vec3 pos) {
	float far = u_nearFar.y;
	float near = u_nearFar.x;
    vec4 clip_space_pos = mul(u_proj, mul(u_view, vec4(pos.xyz, 1.0)));
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0;
    float linearDepth = (near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between 0.01 and 100
    return linearDepth / far; // normalize
}

void main()
{
	float t = -v_nearPoint.y / (v_farPoint.y - v_nearPoint.y);
	vec3 fragPos3D = v_nearPoint + t * (v_farPoint - v_nearPoint);

	float mult = float(t>0.0);
	vec4 colorScale = grid(fragPos3D, 1, 0.2, 1.0) * mult;
	vec4 colorScaleBig = grid(fragPos3D, 10, 0.2, 1.0) * mult;

	gl_FragDepth = computeDepth(fragPos3D);
	
	float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.5 - linearDepth));

	gl_FragColor = colorScale +  colorScaleBig;	
	gl_FragColor.a *= fading;
}	