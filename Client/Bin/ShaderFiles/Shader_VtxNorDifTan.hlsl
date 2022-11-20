//
//matrix         g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
//
//float4         g_WorldLightPosition = { 500.f, 500.f, -500.f, 1.f };
//float4         g_WorldCamPosition;
//
//float3         g_LightColor = { 0.7f, 0.7f, 1.f };
//
//texture2D      g_Texture;
//texture2D      g_DiffuseTexture;
//texture2D      g_NormalTexture;
//
//struct VS_INPUT
//{
//	float3      vPosition : POSITION;
//	float3      vNormal : NORMAL;
//	float2      vTexUV : TEXCOORD0;
//	float3      vTangent : TANGENT;
//	float3      vBinormal : BINORMAL;
//};
//
//struct VS_OUTPUT
//{
//	float4      vPosition : SV_POSITION;
//	float2      vTexUV : TEXCOORD0;
//	float3      vlightDir: TEXCOORD1;
//	float3      vViewDir: TEXCOORD2;
//	float3      T: TEXCOORD3;
//	float3      B: TEXCOORD4;
//	float3      N: TEXCOORD5;
//};
//
//VS_OUTPUT VS_MAIN(VS_INPUT Input)
//{
//	VS_OUTPUT      Output = (VS_OUTPUT)0;
//
//	matrix      matWV, matWVP;
//	matWV = mul(g_WorldMatrix, g_ViewMatrix);
//	matWVP = mul(matWV, g_ProjMatrix);
//
//	Output.vPosition = mul(float4(Input.vPosition, 1.f), matWVP);
//	Output.vTexUV = Input.vTexUV;
//
//	float4 worldPosition = mul(float4(Input.vPosition, 1.f), g_WorldMatrix);
//	float3 lightDir = worldPosition.xyz - g_WorldLightPosition.xyz;
//	Output.vlightDir = normalize(lightDir);
//
//	float3 viewDir = normalize(worldPosition.xyz - g_WorldCamPosition.xyz);
//	Output.vViewDir = viewDir;
//
//	float3 worldNormal = mul(Input.vNormal, (float3x3)g_WorldMatrix);
//	Output.N = normalize(worldNormal);
//
//	float3 worldTargent = mul(Input.vTangent, (float3x3)g_WorldMatrix);
//	Output.T = normalize(worldTargent);
//
//	float3 worldBinormal = mul(Input.vBinormal, (float3x3)g_WorldMatrix);
//	Output.B = normalize(worldBinormal);
//
//	return Output;
//}
//
//struct PS_INPUT
//{
//	float4      vPosition : SV_POSITION;
//	float2      vTexUV : TEXCOORD0;
//	float3      vlightDir: TEXCOORD1;
//	float3      vViewDir: TEXCOORD2;
//	float3      T: TEXCOORD3;
//	float3      B: TEXCOORD4;
//	float3      N: TEXCOORD5;
//};
//
//struct PS_OUTPUT
//{
//	float4      vColor : SV_TARGET0;
//};
//
//PS_OUTPUT main(PS_INPUT Input)
//{
//	PS_OUTPUT      Output = (PS_OUTPUT)0;
//
//	float3 tangentNormal = g_NormalTexture.Sample(NormalSampler, Input.vTexUV).xyz;
//	tangentNormal = normalize(tangentNormal * 2 - 1);
//
//	float3x3 TBN = float3x3(normalize(Input.T), normalize(Input.B), normalize(Input.N));
//	float3 worldNormal = mul(transpose(TBN), tangentNormal);
//
//	float3 albedo = g_Texture.Sample(LinearSampler, Input.vTexUV);
//	float3 lightDir = normalize(Input.vlightDir);
//	float3 diffuse = saturate(dot(worldNormal, -lightDir));
//	diffuse = g_LightColor * albedo.rgb * diffuse;
//
//	float3 specular = 0;
//	if (diffuse.x > 0)
//	{
//		float3 reflection = reflect(lightDir, worldNormal);
//		float3 viewDir = normalize(Input.vViewDir);
//
//		specular = saturate(dot(reflection, -viewDir));
//		specular = pow(specular, 20.0f);
//
//		float4 specularIntensity = g_Texture.Sample(DiffuseSampler, Input.vTexUV);
//		specular *= specularIntensity.rgb * g_LightColor;
//	}
//
//	float3 ambient = float3(0.25f, 0.25f, 0.25f) * albedo;
//	Output.vColor = float4(ambient + diffuse + specular, 1);
//
//	return Output;
//}
//
//technique11 DefaultTechnique
//{
//	pass Default
//	{
//		VertexShader = compile vs_5_0 VS_MAIN();
//		GeometryShader = NULL;
//		HullShader = NULL;
//		DomainShader = NULL;
//		PixelShader = compile ps_5_0 main();
//	}
//}