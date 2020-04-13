#version 450

in vec2 tex_coord;

out vec4 frag_color[2];

layout(binding = 0) uniform sampler2D input_texure;

vec4 cubic(float v) {
    vec4 n = vec4(1.0, 2.0, 3.0, 4.0) - v;
    vec4 s = n * n * n;
    float x = s.x;
    float y = s.y - 4.0 * s.x;
    float z = s.z - 4.0 * s.y + 6.0 * s.x;
    float w = 6.0 - x - y - z;
    return vec4(x, y, z, w) / 6.0;
}

vec4 textureBicubic(sampler2D sampler, vec2 tex_coords) {
    vec2 tex_size = textureSize(sampler, 0);
    vec2 inv_tex_size = 1.0 / tex_size;

    tex_coords = tex_coords * tex_size - 0.5;

    vec2 fxy = fract(tex_coords);
    tex_coords -= fxy;

    vec4 xcubic = cubic(fxy.x);
    vec4 ycubic = cubic(fxy.y);

    vec4 c = tex_coords.xxyy + vec2(-0.5, +1.5).xyxy;

    vec4 s = vec4(xcubic.xz + xcubic.yw, ycubic.xz + ycubic.yw);
    vec4 offset = c + vec4(xcubic.yw, ycubic.yw) / s;

    offset *= inv_tex_size.xxyy;

    vec4 sample0 = texture(sampler, offset.xz);
    vec4 sample1 = texture(sampler, offset.yz);
    vec4 sample2 = texture(sampler, offset.xw);
    vec4 sample3 = texture(sampler, offset.yw);

    float sx = s.x / (s.x + s.y);
    float sy = s.z / (s.z + s.w);

    return mix(mix(sample3, sample2, sx), mix(sample1, sample0, sx), sy);
}

float ColorDist(vec4 a, vec4 b) {
    // https://en.wikipedia.org/wiki/YCbCr#ITU-R_BT.2020_conversion
    const vec3 K = vec3(0.2627, 0.6780, 0.0593);
    const float luminance_weight = .6;
    const mat3 MATRIX = mat3(K * luminance_weight, -.5 * K.r / (1.0 - K.b), -.5 * K.g / (1.0 - K.b), .5, .5,
                             -.5 * K.g / (1.0 - K.r), -.5 * K.b / (1.0 - K.r));
    vec4 diff = a - b;
    vec3 YCbCr = diff.rgb * MATRIX;
    float d = length(YCbCr) * length(vec3(1.0)) / length(vec3(luminance_weight, 1.0, 1.0));
    return sqrt(a.a * b.a * d * d + diff.a * diff.a);
}

const int radius = 2;

float GenerateMaxDiff() {
    float max_diff = 0.0;
    for(int y = -radius; y <= radius; ++y) {
		for(int x = -radius; x <= radius; ++x) {
            vec2 offset = vec2(x, y);
			float weight = pow(length(offset), -length(offset));
            max_diff += weight;
	    }
	}
    return max_diff;
}

const float max_diff = GenerateMaxDiff();
const bool outline = true;

void main() {
	vec4 center_texel = textureLod(input_texure, tex_coord, 0.0);
	vec2 final_offset = vec2(0.0);
    float total_diff = 0.0;
	for(int y = -radius; y <= radius; ++y) {
		for(int x = -radius; x <= radius; ++x) {
        	if (0 == (x | y)) continue;
			vec2 offset = vec2(x, y);
			float weight = pow(length(offset), -length(offset));
			vec4 texel = textureOffset(input_texure, tex_coord, ivec2(x, y));
            float diff = ColorDist(texel, center_texel) * weight;
            total_diff += diff;
			final_offset += diff * offset;
	    }
	}
    frag_color[1] = vec4(abs(final_offset), final_offset.x * final_offset.y, 1.0);
	final_offset /= textureSize(input_texure, 0);
    vec4 offset_color = textureBicubic(input_texure, tex_coord - final_offset);
    float black_level = 1.0 - smoothstep(max_diff - sqrt(max_diff), max_diff + sqrt(max_diff), total_diff * total_diff);
    frag_color[0] = vec4(offset_color.rgb * black_level, offset_color.a);
//    if (length(final_offset) < 1 / sqrt(max_offset) && total_diff > sqrt(max_offset)) {
//        frag_color[0] = vec4(0.0, 0.0, 0.0, 1.0);
//    } else {
//        frag_color[0] = textureBicubic(input_texure, tex_coord - final_offset);
//    }

}
