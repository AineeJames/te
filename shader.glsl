#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform sampler2D fontAtlasTexture;
uniform sampler2D gridTexture;
uniform ivec2 gridSize;
uniform ivec2 cellSize;

// Standard VGA 16-color palette
const vec3 vgaPalette[16] = vec3[](
        vec3(0.00, 0.00, 0.00), // 0: Black
        vec3(0.00, 0.00, 0.67), // 1: Blue
        vec3(0.00, 0.67, 0.00), // 2: Green
        vec3(0.00, 0.67, 0.67), // 3: Cyan
        vec3(0.67, 0.00, 0.00), // 4: Red
        vec3(0.67, 0.00, 0.67), // 5: Magenta
        vec3(0.67, 0.33, 0.00), // 6: Brown/Yellow
        vec3(0.67, 0.67, 0.67), // 7: Light Gray
        vec3(0.33, 0.33, 0.33), // 8: Dark Gray
        vec3(0.33, 0.33, 1.00), // 9: Light Blue
        vec3(0.33, 1.00, 0.33), // 10: Light Green
        vec3(0.33, 1.00, 1.00), // 11: Light Cyan
        vec3(1.00, 0.33, 0.33), // 12: Light Red
        vec3(1.00, 0.33, 1.00), // 13: Light Magenta
        vec3(1.00, 1.00, 0.33), // 14: Yellow
        vec3(1.00, 1.00, 1.00) // 15: White
    );

void main() {
    // Sample grid data
    vec4 gridSample = texture(gridTexture, fragTexCoord);
    int glyph = int(gridSample.r * 255.0);
    int fgColor = int(gridSample.g * 255.0);
    int bgColor = int(gridSample.b * 255.0);

    // Clamp to valid VGA palette indices (0-15)
    fgColor = fgColor & 15;
    bgColor = bgColor & 15;

    // Determine position within the cell
    vec2 cellUV = fract(fragTexCoord * vec2(gridSize));

    // Decode CP437: 16x16 grid in font atlas
    int gx = glyph & 15;
    int gy = glyph >> 4;

    // Calculate position in font atlas
    vec2 atlasUV = (vec2(gx, gy) + cellUV) / 16.0;

    // Sample from font atlas
    vec4 fontSample = texture(fontAtlasTexture, atlasUV);

    // Get colors from palette
    vec3 fgRGB = vgaPalette[fgColor];
    vec3 bgRGB = vgaPalette[bgColor];

    // Blend: use font alpha to interpolate between bg and fg
    vec3 color = mix(bgRGB, fgRGB, fontSample.r);

    finalColor = vec4(color, 1.0);
}

