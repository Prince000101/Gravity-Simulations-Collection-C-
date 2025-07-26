// lens_distortion.frag
uniform sampler2D texture;
uniform vec2 resolution;
uniform vec2 blackHolePos;
uniform float time;

void main() {
    vec2 uv = gl_FragCoord.xy / resolution;
    vec2 bh = blackHolePos / resolution;

    vec2 dir = uv - bh;
    float r = length(dir);

    float strength = 0.25;
    float distortion = strength / (r + 0.1); // avoid divide-by-zero

    vec2 offset = dir * distortion;

    // chromatic aberration (RGB channels offset slightly)
    vec3 col;
    col.r = texture2D(texture, uv - offset * 0.98).r;
    col.g = texture2D(texture, uv - offset).g;
    col.b = texture2D(texture, uv - offset * 1.02).b;

    gl_FragColor = vec4(col, 1.0);
}
