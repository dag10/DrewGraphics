// This file is linked into every fragment shader.
// All fragment shaders should implement frag(), not main().

// If we're rendering this model through a portal, this mat4 is the
// inverse of the transform of the output portal-front in scene space.
// This is used to transform a fragment's position from scene-space to
// portal-space to determine which side of the portal we're on for culling.
uniform mat4 _InvPortal;

vec4 frag();

void main() {
  if (_InvPortal[3][3] == 1 && (_InvPortal * v_ScenePos).z < 0) {
    discard;
  }

  FragColor = frag();
}

