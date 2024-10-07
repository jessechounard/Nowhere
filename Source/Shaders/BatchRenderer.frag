varying vec4 v_color;
varying vec2 v_texcoord;

// custom input from program
uniform sampler2D TextureSampler;

void main()
{
	gl_FragColor = texture2D(TextureSampler, v_texcoord) * v_color;
}
