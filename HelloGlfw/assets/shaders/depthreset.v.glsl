vec4 vert() {
  // The Quad mesh goes from -0.5 to 0.5, so double the coordinates
  // to fill the normalized device coordinates.
  vec4 pos = vec4(in_Position * 2.0, 1.0);

  // Depth is farthest possible.
  pos.z = 1;
  
  return pos;
}

