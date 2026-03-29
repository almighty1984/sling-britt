//#version 440

uniform sampler2D texture;

uniform vec3 color;

uniform vec2 light_pos;

void main() {
    // lookup the pixel in the texture
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    // multiply it by the color
    gl_FragColor = pixel * vec4(color.r / 255.0, color.g / 255.0, color.b / 255.0, 1.0);
}

// vec4 vert_pos;

// uniform sampler2D texture;

// uniform vec2 light_pos;

// void main()
// {
// 	//Ambient light
// 	vec4 ambient = vec4(0.75, 0.75, 0.75, 0.0);

// 	vec2 calc_light_pos = (gl_ModelViewProjectionMatrix * vec4(light_pos, 0, 1)).xy;
	
// 	// lookup the pixel in the texture
// 	//vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

// 	vec4 to_output =  vec4(0.0);

// 	vec2 light_pos2 = calc_light_pos;	
	
// 	vec2 light_to_frag = light_pos2 - vert_pos.xy;

// 	for (int i = 0; i < 1; i++) {
// 		//Convert light to view coords
		
// 		//Calculate the vector from light to pixel (Make circular)
		
// 		light_to_frag.x = light_to_frag.x;
// 		light_to_frag.y = light_to_frag.y / 1.7;


// 		//Length of the vector (distance)
		

		

		

// 		//float vec_length = clamp(length(light_to_frag) * 2, 0, 1);
//         float vec_length = 1;

// 		to_output += ambient + vec4(1-vec_length, 1-vec_length, 1-vec_length, 1);
		
// 	}

	
	


//     // multiply it by the color and lighting
// 	//if (hasTexture == true)
// 	//{
// 		gl_FragColor = texture2D(texture, gl_TexCoord[0].xy) * to_output;
// 	//}
// 	//else
// 	//{
// 	//	gl_FragColor = gl_Color;
// 	//}
// }