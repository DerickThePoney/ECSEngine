$input v_nearPoint, v_farPoint

#include <bgfx_shader.sh>

vec4 grid(vec3 fragPos3D, float scale, float baseGrey, float baseAxisMult) {
    vec2 coord = fragPos3D.xz / scale; // use the scale variable to set the distance between the lines
    vec2 derivative = fwidth(coord);
    
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float l = min(grid.x, grid.y);
    vec4 color = vec4(baseGrey, baseGrey, baseGrey, 1.0 - min(l, 1.0));
    // z axis
    if(coord.x > -0.1 * baseAxisMult && coord.x < 0.1 * baseAxisMult)
    {
    	color.z = clamp((1.0 - min(grid.x, 1.0)) * 10,0,1);
    	color.xy = vec2(0.0,0.0);
    	color.a *= baseAxisMult;
    }
    // x axis
    if(coord.y > -0.1 * baseAxisMult && coord.y < 0.1 * baseAxisMult)
    {
    	color.x = clamp((1.0 - min(grid.y, 1.0)) * 10,0,1);
    	color.yz = vec2(0.0,0.0);
    	color.a *= baseAxisMult;
    }
    return color;
}
float computeDepth(vec3 pos) {
    vec4 clip_space_pos = mul(u_proj, mul(u_view, vec4(pos.xyz, 1.0)));
    return (clip_space_pos.z / clip_space_pos.w);
}

float computeLinearDepth(vec3 pos) {
	float far = 50.0;
	float near = 1.0;
    vec4 clip_space_pos = mul(u_proj, mul(u_view, vec4(pos.xyz, 1.0)));
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w);
    return clip_space_depth;
    float linearDepth = (near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between 0.01 and 100
    return linearDepth / far; // normalize
}

void main()
{
	float t = -v_nearPoint.y / (v_farPoint.y - v_nearPoint.y);
	vec3 fragPos3D = v_nearPoint + t * (v_farPoint - v_nearPoint);

	float mult = float(t>0.0);
	vec4 colorScale = grid(fragPos3D, 1, 0.5, 1.0) * mult;
	vec4 colorScaleBig = grid(fragPos3D, 10, 1, 0.0) * mult;

	gl_FragDepth = computeDepth(fragPos3D);
	
	float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.5 - linearDepth));

	gl_FragColor = mix(colorScale, colorScaleBig, colorScaleBig.a);//vec4(1.0, 0.0, 0.0, 1.0 * float(t > 0.0));max(colorScale, colorScaleBig)
	//gl_FragColor = vec4_splat(fading);
	//gl_FragColor.a = 1.0;
}	