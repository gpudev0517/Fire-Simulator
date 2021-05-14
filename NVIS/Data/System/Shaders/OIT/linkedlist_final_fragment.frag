#version 430 core

#define MAX_FRAGMENTS 75

struct NodeType {
  vec4 color;
  float depth;
  uint next;
};

uniform sampler2D uBackgroundTex;

layout( binding = 0, r32ui) uniform uimage2D headPointers;
layout( binding = 0, std430 ) buffer BufferObject {
  NodeType nodes[];
};

in vec2 texCoord;
out vec4 FragColor;


void main()
{
  	NodeType frags[MAX_FRAGMENTS];

	int count = 0;

	// Get the index of the head of the list
	uint n = imageLoad(headPointers, ivec2(gl_FragCoord.xy)).r;

	// Copy the linked list for this fragment into an array
	while( n != 0xffffffff && count < MAX_FRAGMENTS) {
	    frags[count] = nodes[n];
	    n = frags[count].next;
	    count++;
	}

  


    //merge sort
    int i, j1, j2, k;
    int a, b, c;
    int step = 1;
    NodeType leftArray[MAX_FRAGMENTS/2]; //for merge sort
    
    while (step <= count)
    {
        i = 0;
        while (i < count - step)
        {
            ////////////////////////////////////////////////////////////////////////
            //merge(step, i, i + step, min(i + step + step, count));
            a = i;
            b = i + step;
            c = (i + step + step) >= count ? count : (i + step + step);

            for (k = 0; k < step; k++)
                leftArray[k] = frags[a + k];
            
            j1 = 0;
            j2 = 0;
            for (k = a; k < c; k++)
            {
                if (b + j1 >= c || (j2 < step && leftArray[j2].depth > frags[b + j1].depth))
                    frags[k] = leftArray[j2++];
                else
                    frags[k] = frags[b + j1++];
            }
            ////////////////////////////////////////////////////////////////////////
            i += 2 * step;
        }
        step *= 2;
    } 

  // Traverse the array, and combine the colors using the alpha
  // channel.
  //vec4 color = vec4(0.5, 0.5, 0.5, 1.0);

  vec4 color = texture(uBackgroundTex, texCoord);
  //vec4 color = vec4(1, 0, 0, 1);
  for( int i = 0; i < count; i++ )
  {
    color = mix( color, frags[i].color, frags[i].color.a);
  }

  // Output the final color
  if (count >= 1)
    gl_FragDepth = frags[count - 1].depth;
  else
    gl_FragDepth = 1.0f;
  FragColor = color;
}