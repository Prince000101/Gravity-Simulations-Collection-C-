// lens_distortion.frag
uniform sampler2D texture;
uniform vec2 blackHolePos; // [0,1]
uniform float time;

void main() {
    vec2 uv = gl_TexCoord[0].xy;
    vec2 delta = uv - blackHolePos;
    float dist = length(delta);

    float radius = 0.2;
    float strength = 0.03;

    vec2 offset = vec2(0.0);
    if (dist < radius) {
        float factor = (1.0 - dist / radius);
        float distortion = strength * factor * factor;
        offset = normalize(delta) * distortion;
    }

    vec4 color;
    color.r = texture2D(texture, uv + offset * 1.02).r;
    color.g = texture2D(texture, uv + offset).g;
    color.b = texture2D(texture, uv - offset * 1.02).b;

    gl_FragColor = color;
}
