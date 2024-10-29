varying vec2 v_texcoord;

uniform vec2 Dimensions;
uniform float Radius;
uniform float Border;

float sdCircle(in vec2 p, in float r) 
{
    return length(p) - r;
}

float sdAnnularCircle(in vec2 p, in float r, in float offset) 
{
    return abs(sdCircle(p, r)) - offset;
}

void main()
{
    vec3 edgeColor = vec3(1.);
    vec3 insideColor = vec3(0.65,0.85,1.0);
    vec3 outsideColor = vec3(0.);

    vec2 p = v_texcoord * 2. - 1.;
    p.x *= Dimensions.x / Dimensions.y;

    float r = Radius / Dimensions.y * 2.;
    float b = Border / Dimensions.y * 2.;
    float d = sdAnnularCircle(p, r, b);

    vec3 col = (d>0.0) ? outsideColor : insideColor;
	col = mix(col, insideColor, 1.0-smoothstep(0.0, 1. / Dimensions.y * 2,abs(d)));

    gl_FragColor = vec4(col, 1.);
/*
    vec3 edgeColor = vec3(1.);
    vec3 insideColor = vec3(0.65,0.85,1.0);
    vec3 outsideColor = vec3(0.);

	vec2 p = v_texcoord * 2. - 1.;
	//p.x *= Dimensions.x / Dimensions.y;

    float r = Radius / Dimensions.y;
    float thickness = Border / Dimensions.y;

    float d = annularCircle(p, r, thickness);
    
    vec3 col = (d>0.0) ? outsideColor : insideColor;
	//col = mix( col, edgeColor, 1.0-smoothstep(0.0,0.01,abs(d)) );

	gl_FragColor = vec4(v_texcoord, 0.,1.0);
*/
}
