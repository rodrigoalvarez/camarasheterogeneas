// PointCloudShader1 : http://unitycoder.com/blog/

Shader "DX11/VertexColorPoints" {
SubShader {

Tags { "RenderType"="Transparent" "Queue"="Transparent" }
Blend SrcAlpha OneMinusSrcAlpha

Pass {

CGPROGRAM
#pragma target 5.0

#pragma vertex vert
#pragma fragment frag
#include "UnityCG.cginc"

StructuredBuffer<float3> buf_Points;
StructuredBuffer<float3> buf_Positions;

struct ps_input {
	float4 pos : SV_POSITION;
};

ps_input vert (uint id : SV_VertexID, uint inst : SV_InstanceID)
{
	ps_input o;
	float3 worldPos = buf_Points[id] + buf_Positions[inst];
	o.pos = mul (UNITY_MATRIX_VP, float4(worldPos,1.0f));
	return o;
}

float4 frag (ps_input i) : COLOR
{

	return float4(1.0f,0.0f,0.0f,0.5); // set point color/alpha here
	
}

ENDCG

}
}

Fallback Off
}
