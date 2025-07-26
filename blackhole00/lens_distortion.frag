// lens_distortion.frag
uniform sampler2D texture;
uniform vec2 blackHolePos; // Normalized [0,1] screen space position
uniform float time;

void main() {
    vec2 uv = gl_TexCoord[0].xy;
    vec2 delta = uv - blackHolePos;
    float dist = length(delta);

    float radius = 0.2;
    float strength = 0.03;

    vec2 distortedUV = uv;

    if (dist < radius) {
        float distortion = strength / (dist + 0.01);
        distortion *= (1.0 - dist / radius);  // Fade out near edge
        distortedUV += normalize(delta) * distortion;
    }

    gl_FragColor = texture2D(texture, distortedUV);
}
