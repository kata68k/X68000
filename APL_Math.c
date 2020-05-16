#ifndef	APL_MATH_C
#define	APL_MATH_C

#include <math.h>

#include "inc/usr_macro.h"
#include "APL_Math.h"

/* �O���[�o���ϐ� */

/*
	sin()
	  0- 90 ��  sin( �� )
	 90-180 ��  sin( 180 - �� )
	180-270 �� -sin( �� - 180 )
	270-360 �� -sin( 360 - �� )

	cos(��) �� sin(�� + 90)
*/

/*
*/
/* Sin�e�[�u�� �z��ԍ���deg */		/* �K�v�ł���ΐ��x���グ�� */
const	SS	t_Sin256[91] = {
/*	  0   1   2   3   4   5   6   7   8   9 */
	 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,	/*   0 -  9 */
	10,11,12,13,14,15,16,17,18,19,	/*  10 - 19 */
	20,21,22,23,24,25,26,27,28,29,	/*  20 - 29 */
	30,31,32,33,34,35,36,37,38,39,	/*  30 - 39 */
	40,41,42,43,44,45,46,47,48,49,	/*  40 - 49 */
	50,51,52,53,54,55,56,57,58,59,	/*  50 - 59 */
	60,61,62,63,64,65,66,67,68,69,	/*  60 - 69 */
	70,71,72,73,74,75,76,77,78,79,	/*  70 - 79 */
	80,81,82,83,84,85,86,87,88,89,	/*  80 - 89 */
	90								/*  90	  */
};


const FLOAT atanTable[] = 
{
0.000000000	,0.055952892	,0.111905677	,0.167858249	,0.223810500	,0.279762325	,0.335713616	,0.391664267	,
0.447614171	,0.503563221	,0.559511311	,0.615458334	,0.671404183	,0.727348752	,0.783291934	,0.839233622	,
0.89517371	,0.951112092	,1.00704866	,1.062983308	,1.118915931	,1.17484642	,1.23077467	,1.286700574	,
1.342624027	,1.39854492	,1.454463149	,1.510378607	,1.566291187	,1.622200783	,1.67810729	,1.7340106	,
1.789910608	,1.845807208	,1.901700293	,1.957589757	,2.013475495	,2.069357401	,2.125235368	,2.181109291	,
2.236979063	,2.29284458	,2.348705736	,2.404562423	,2.460414539	,2.516261975	,2.572104628	,2.627942391	,
2.683775159	,2.739602828	,2.79542529	,2.851242442	,2.907054178	,2.962860393	,3.018660982	,3.07445584	,
3.130244862	,3.186027943	,3.241804979	,3.297575864	,3.353340494	,3.409098765	,3.464850572	,3.52059581	,
3.576334375	,3.632066163	,3.68779107	,3.743508991	,3.799219823	,3.854923461	,3.910619802	,3.966308742	,
4.021990177	,4.077664003	,4.133330118	,4.188988416	,4.244638796	,4.300281154	,4.355915386	,4.411541389	,
4.467159061	,4.522768299	,4.578368999	,4.63396106	,4.689544377	,4.74511885	,4.800684375	,4.85624085	,
4.911788173	,4.967326241	,5.022854953	,5.078374207	,5.133883901	,5.189383933	,5.244874202	,5.300354605	,
5.355825043	,5.411285413	,5.466735614	,5.522175546	,5.577605107	,5.633024196	,5.688432713	,5.743830558	,
5.799217629	,5.854593827	,5.909959051	,5.965313201	,6.020656177	,6.075987879	,6.131308208	,6.186617064	,
6.241914347	,6.297199959	,6.352473799	,6.407735769	,6.46298577	,6.518223703	,6.57344947	,6.628662971	,
6.683864108	,6.739052783	,6.794228899	,6.849392356	,6.904543057	,6.959680904	,7.014805801	,7.069917648	,
7.125016349	,7.180101807	,7.235173924	,7.290232605	,7.345277751	,7.400309267	,7.455327056	,7.510331022	,
7.565321068	,7.6202971	,7.675259019	,7.730206733	,7.785140143	,7.840059156	,7.894963676	,7.949853608	,
8.004728857	,8.059589329	,8.114434928	,8.16926556	,8.224081132	,8.278881548	,8.333666716	,8.388436541	,
8.443190929	,8.497929788	,8.552653023	,8.607360543	,8.662052253	,8.716728062	,8.771387876	,8.826031603	,
8.880659151	,8.935270427	,8.989865341	,9.044443799	,9.099005711	,9.153550985	,9.20807953	,9.262591255	,
9.317086069	,9.371563881	,9.426024602	,9.480468139	,9.534894404	,9.589303307	,9.643694757	,9.698068665	,
9.752424942	,9.806763498	,9.861084244	,9.915387093	,9.969671954	,10.02393874	,10.07818736	,10.13241773	,
10.18662976	,10.24082336	,10.29499845	,10.34915493	,10.40329273	,10.45741175	,10.5115119	,10.56559311	,
10.61965528	,10.67369832	,10.72772216	,10.78172671	,10.83571187	,10.88967757	,10.94362372	,10.99755023	,
11.05145703	,11.10534402	,11.15921112	,11.21305824	,11.26688531	,11.32069224	,11.37447895	,11.42824535	,
11.48199135	,11.53571689	,11.58942186	,11.6431062	,11.69676982	,11.75041263	,11.80403456	,11.85763552	,
11.91121543	,11.96477421	,12.01831179	,12.07182807	,12.12532297	,12.17879643	,12.23224835	,12.28567866	,
12.33908728	,12.39247412	,12.44583912	,12.49918218	,12.55250323	,12.6058022	,12.659079	,12.71233355	,
12.76556578	,12.81877561	,12.87196296	,12.92512776	,12.97826992	,13.03138938	,13.08448604	,13.13755985	,
13.19061071	,13.24363856	,13.29664332	,13.34962491	,13.40258326	,13.4555183	,13.50842994	,13.56131811	,
13.61418274	,13.66702376	,13.71984109	,13.77263466	,13.82540438	,13.8781502	,13.93087204	,13.98356982	,
14.03624347	,14.08889292	,14.14151809	,14.19411892	,14.24669534	,14.29924726	,14.35177463	,14.40427737	,
14.4567554	,14.50920866	,14.56163708	,14.61404059	,14.66641911	,14.71877259	,14.77110094	,14.8234041	,
14.875682	,14.92793458	,14.98016175	,15.03236347	,15.08453965	,15.13669022	,15.18881514	,15.24091431	,
15.29298769	,15.34503519	,15.39705676	,15.44905233	,15.50102183	,15.55296519	,15.60488236	,15.65677326	,
15.70863783	,15.760476	,15.81228772	,15.86407291	,15.91583152	,15.96756347	,16.01926871	,16.07094716	,
16.12259878	,16.17422349	,16.22582123	,16.27739194	,16.32893555	,16.38045202	,16.43194126	,16.48340323	,
16.53483786	,16.58624508	,16.63762485	,16.68897709	,16.74030175	,16.79159877	,16.84286809	,16.89410964	,
16.94532338	,16.99650923	,17.04766714	,17.09879706	,17.14989892	,17.20097266	,17.25201824	,17.30303558	,
17.35402464	,17.40498535	,17.45591766	,17.50682151	,17.55769684	,17.60854361	,17.65936175	,17.71015121	,
17.76091192	,17.81164385	,17.86234693	,17.9130211	,17.96366632	,18.01428252	,18.06486966	,18.11542768	,
18.16595653	,18.21645615	,18.26692649	,18.3173675	,18.36777912	,18.41816131	,18.46851401	,18.51883717	,
18.56913073	,18.61939465	,18.66962888	,18.71983336	,18.77000804	,18.82015288	,18.87026782	,18.92035281	,
18.97040781	,19.02043276	,19.07042762	,19.12039233	,19.17032685	,19.22023113	,19.27010512	,19.31994877	,
19.36976203	,19.41954487	,19.46929723	,19.51901906	,19.56871031	,19.61837095	,19.66800092	,19.71760018	,
19.76716868	,19.81670637	,19.86621322	,19.91568918	,19.9651342	,20.01454823	,20.06393124	,20.11328317	,
20.16260399	,20.21189365	,20.26115211	,20.31037932	,20.35957525	,20.40873984	,20.45787307	,20.50697487	,
20.55604522	,20.60508407	,20.65409138	,20.7030671	,20.75201121	,20.80092365	,20.84980439	,20.89865338	,
20.94747059	,20.99625597	,21.04500949	,21.09373111	,21.14242078	,21.19107848	,21.23970415	,21.28829777	,
21.33685929	,21.38538868	,21.43388589	,21.4823509	,21.53078366	,21.57918414	,21.62755229	,21.67588809	,
21.7241915	,21.77246248	,21.82070099	,21.86890701	,21.91708049	,21.96522139	,22.0133297	,22.06140536	,
22.10944834	,22.15745862	,22.20543616	,22.25338092	,22.30129287	,22.34917197	,22.3970182	,22.44483152	,
22.4926119	,22.5403593	,22.5880737	,22.63575506	,22.68340335	,22.73101854	,22.7786006	,22.82614949	,
22.87366519	,22.92114767	,22.96859688	,23.01601282	,23.06339544	,23.11074471	,23.15806061	,23.20534311	,
23.25259217	,23.29980778	,23.34698989	,23.39413849	,23.44125354	,23.48833502	,23.53538289	,23.58239714	,
23.62937773	,23.67632464	,23.72323784	,23.7701173	,23.816963	,23.86377491	,23.910553	,23.95729726	,
24.00400765	,24.05068415	,24.09732673	,24.14393537	,24.19051005	,24.23705073	,24.28355741	,24.33003004	,
24.37646862	,24.42287311	,24.46924349	,24.51557973	,24.56188183	,24.60814975	,24.65438346	,24.70058296	,
24.74674821	,24.7928792	,24.8389759	,24.88503829	,24.93106635	,24.97706006	,25.0230194	,25.06894435	,
25.11483489	,25.16069099	,25.20651264	,25.25229982	,25.2980525	,25.34377068	,25.38945432	,25.43510342	,
25.48071795	,25.52629789	,25.57184323	,25.61735394	,25.66283002	,25.70827143	,25.75367817	,25.79905022	,
25.84438755	,25.88969016	,25.93495803	,25.98019114	,26.02538946	,26.070553	,26.11568173	,26.16077563	,
26.2058347	,26.2508589	,26.29584824	,26.3408027	,26.38572225	,26.43060689	,26.4754566	,26.52027137	,
26.56505118	,26.60979602	,26.65450587	,26.69918073	,26.74382058	,26.7884254	,26.83299519	,26.87752993	,
26.92202961	,26.96649421	,27.01092373	,27.05531815	,27.09967746	,27.14400165	,27.18829071	,27.23254462	,
27.27676338	,27.32094698	,27.3650954	,27.40920863	,27.45328667	,27.4973295	,27.54133712	,27.58530951	,
27.62924667	,27.67314858	,27.71701524	,27.76084664	,27.80464276	,27.84840361	,27.89212917	,27.93581943	,
27.97947439	,28.02309403	,28.06667836	,28.11022736	,28.15374103	,28.19721935	,28.24066232	,28.28406994	,
28.3274422	,28.37077909	,28.4140806	,28.45734674	,28.50057748	,28.54377284	,28.58693279	,28.63005735	,
28.67314649	,28.71620022	,28.75921853	,28.80220142	,28.84514888	,28.8880609	,28.93093749	,28.97377864	,
29.01658434	,29.0593546	,29.1020894	,29.14478875	,29.18745264	,29.23008106	,29.27267403	,29.31523152	,
29.35775354	,29.40024009	,29.44269117	,29.48510677	,29.52748689	,29.56983152	,29.61214068	,29.65441435	,
29.69665254	,29.73885524	,29.78102245	,29.82315417	,29.8652504	,29.90731115	,29.9493364	,29.99132616	,
30.03328044	,30.07519922	,30.11708251	,30.15893031	,30.20074262	,30.24251944	,30.28426078	,30.32596662	,
30.36763698	,30.40927186	,30.45087125	,30.49243516	,30.53396359	,30.57545655	,30.61691402	,30.65833602	,
30.69972255	,30.74107361	,30.7823892	,30.82366933	,30.864914	,30.90612321	,30.94729696	,30.98843526	,
31.02953811	,31.07060552	,31.11163748	,31.15263401	,31.1935951	,31.23452077	,31.27541101	,31.31626582	,
31.35708522	,31.39786921	,31.4386178	,31.47933098	,31.52000876	,31.56065115	,31.60125816	,31.64182978	,
31.68236603	,31.72286691	,31.76333242	,31.80376258	,31.84415738	,31.88451684	,31.92484096	,31.96512975	,
32.00538321	,32.04560135	,32.08578418	,32.1259317	,32.16604393	,32.20612086	,32.24616251	,32.28616889	,
32.32613999	,32.36607584	,32.40597643	,32.44584178	,32.48567189	,32.52546677	,32.56522644	,32.60495089	,
32.64464013	,32.68429419	,32.72391306	,32.76349675	,32.80304527	,32.84255864	,32.88203686	,32.92147994	,
32.96088788	,33.00026071	,33.03959843	,33.07890105	,33.11816858	,33.15740102	,33.1965984	,33.23576072	,
33.27488798	,33.31398021	,33.35303742	,33.3920596	,33.43104678	,33.46999896	,33.50891616	,33.54779839	,
33.58664565	,33.62545797	,33.66423535	,33.7029778	,33.74168533	,33.78035796	,33.81899571	,33.85759857	,
33.89616656	,33.9346997	,33.973198	,34.01166147	,34.05009013	,34.08848398	,34.12684303	,34.16516731	,
34.20345683	,34.24171159	,34.27993161	,34.31811691	,34.3562675	,34.39438338	,34.43246458	,34.47051111	,
34.50852299	,34.54650022	,34.58444282	,34.6223508	,34.66022418	,34.69806298	,34.7358672	,34.77363687	,
34.81137199	,34.84907258	,34.88673866	,34.92437024	,34.96196734	,34.99952997	,35.03705814	,35.07455187	,
35.11201118	,35.14943609	,35.1868266	,35.22418273	,35.2615045	,35.29879193	,35.33604502	,35.37326381	,
35.41044829	,35.44759849	,35.48471443	,35.52179612	,35.55884358	,35.59585682	,35.63283585	,35.66978071	,
35.7066914	,35.74356794	,35.78041035	,35.81721864	,35.85399283	,35.89073294	,35.92743899	,35.96411099	,
36.00074895	,36.03735291	,36.07392287	,36.11045885	,36.14696088	,36.18342896	,36.21986312	,36.25626337	,
36.29262973	,36.32896222	,36.36526086	,36.40152567	,36.43775665	,36.47395385	,36.51011726	,36.54624691	,
36.58234282	,36.61840501	,36.65443349	,36.69042829	,36.72638942	,36.7623169	,36.79821075	,36.83407099	,
36.86989765	,36.90569073	,36.94145025	,36.97717625	,37.01286873	,37.04852771	,37.08415322	,37.11974528	,
37.1553039	,37.19082911	,37.22632091	,37.26177935	,37.29720442	,37.33259617	,37.36795459	,37.40327972	,
37.43857157	,37.47383017	,37.50905553	,37.54424768	,37.57940663	,37.61453241	,37.64962504	,37.68468453	,
37.71971092	,37.75470421	,37.78966443	,37.8245916	,37.85948574	,37.89434687	,37.92917502	,37.9639702	,
37.99873244	,38.03346176	,38.06815817	,38.1028217	,38.13745238	,38.17205021	,38.20661523	,38.24114746	,
38.27564691	,38.31011361	,38.34454758	,38.37894884	,38.41331742	,38.44765333	,38.4819566	,38.51622725	,
38.5504653	,38.58467077	,38.61884369	,38.65298408	,38.68709196	,38.72116735	,38.75521027	,38.78922076	,
38.82319882	,38.85714448	,38.89105777	,38.92493871	,38.95878731	,38.99260361	,39.02638762	,39.06013937	,
39.09385889	,39.12754618	,39.16120128	,39.19482421	,39.228415	,39.26197366	,39.29550021	,39.32899469	,
39.36245712	,39.39588751	,39.42928589	,39.46265229	,39.49598672	,39.52928922	,39.5625598	,39.59579848	,
39.6290053	,39.66218028	,39.69532343	,39.72843479	,39.76151437	,39.7945622	,39.8275783	,39.86056271	,
39.89351543	,39.92643649	,39.95932593	,39.99218376	,40.02501	,40.05780469	,40.09056784	,40.12329947	,
40.15599962	,40.18866831	,40.22130556	,40.25391139	,40.28648584	,40.31902891	,40.35154065	,40.38402107	,
40.41647019	,40.44888805	,40.48127466	,40.51363005	,40.54595424	,40.57824727	,40.61050915	,40.64273991	,
40.67493957	,40.70710815	,40.73924569	,40.77135221	,40.80342773	,40.83547228	,40.86748587	,40.89946855	,
40.93142032	,40.96334122	,40.99523128	,41.02709051	,41.05891894	,41.09071659	,41.1224835	,41.15421968	,
41.18592517	,41.21759998	,41.24924414	,41.28085768	,41.31244062	,41.34399299	,41.37551481	,41.40700611	,
41.43846691	,41.46989724	,41.50129713	,41.53266659	,41.56400566	,41.59531436	,41.62659271	,41.65784074	,
41.68905849	,41.72024596	,41.75140319	,41.7825302	,41.81362703	,41.84469368	,41.8757302	,41.9067366	,
41.93771291	,41.96865917	,41.99957538	,42.03046158	,42.0613178	,42.09214405	,42.12294037	,42.15370679	,
42.18444332	,42.21514999	,42.24582683	,42.27647387	,42.30709113	,42.33767863	,42.36823641	,42.39876449	,
42.42926289	,42.45973164	,42.49017077	,42.5205803	,42.55096026	,42.58131068	,42.61163157	,42.64192298	,
42.67218491	,42.70241741	,42.73262049	,42.76279418	,42.79293851	,42.8230535	,42.85313918	,42.88319558	,
42.91322272	,42.94322063	,42.97318934	,43.00312887	,43.03303924	,43.06292049	,43.09277263	,43.12259571	,
43.15238973	,43.18215474	,43.21189075	,43.24159779	,43.27127589	,43.30092508	,43.33054537	,43.3601368	,
43.3896994	,43.41923318	,43.44873818	,43.47821443	,43.50766194	,43.53708075	,43.56647088	,43.59583236	,
43.62516522	,43.65446948	,43.68374516	,43.7129923	,43.74221092	,43.77140105	,43.80056271	,43.82969593	,
43.85880074	,43.88787717	,43.91692523	,43.94594496	,43.97493639	,44.00389953	,44.03283442	,44.06174109	,
44.09061955	,44.11946984	,44.14829198	,44.17708601	,44.20585194	,44.2345898	,44.26329962	,44.29198142	,
44.32063524	,44.34926110	,44.37785903	,44.40642905	,44.43497118	,44.46348546	,44.49197192	,44.52043057	,
44.54886145	,44.57726459	,44.60564	,44.63398771	,44.66230776	,44.69060017	,44.71886497	,44.74710217	,
44.77531182	,44.80349393	,44.83164854	,44.85977566	,44.88787533	,44.91594757	,44.94399241	,44.97200988	,
45.00000000	
};
	
/* �֐��̃v���g�^�C�v�錾 */
FLOAT APL_Atan2(FLOAT , FLOAT );
SS APL_Cos(SS);
SS APL_Sin(SS);
SS APL_Tan256(SS);

/* �֐� */

/* �Q�l https://mitoshiropj.blogspot.com/2013/08/blog-post_9340.html */
FLOAT APL_Atan2(FLOAT y, FLOAT x)
{
	const SI atanDevide = 1024;
	SI	num;
	FLOAT r, d;
	
	if (x >= 0)
	{
		if (y >= 0)
		{
			if (y >= x)
			{
				// 0-45
				r = x / y;
				num = (SI)(r * atanDevide);
				d = atanTable[num];
//				printf("0-45 (%d)= %f, %f\n", num, r, d);
				return d;
			}
			else
			{
				// 45-90
				r = y / x;
				num = (SI)(r * atanDevide);
				d = atanTable[num];
//				printf("45-90 (%d)= %f, %f\n", num, r, d);
				return 90.0f - d;
			}
		}
		else
		{
			if (x >= -y)
			{
				// 90-135
				r = -y / x;
				num = (SI)(r * atanDevide);
				d = atanTable[num];
//				printf("90-135 (%d)= %f, %f\n", num, r, d);
				return 90.0f + d;
			}
			else
			{
				// 135-180
				r = x / -y;
				num = (SI)(r * atanDevide);
				d = atanTable[num];
//				printf("135-180 (%d)= %f, %f\n", num, r, d);
				return 180.0f - d;
			}
		}
	}
	else
	{
		if (y < 0)
		{
			if (-y >= -x)
			{
				// 180-225
				r = -x / -y;
				num = (SI)(r * atanDevide);
				d = atanTable[num];
//				printf("180-225 (%d)= %f, %f\n", num, r, d);
				return 180.0f + d;
			}
			else
			{
				// 225-270
				r = -y / -x;
				num = (SI)(r * atanDevide);
				d = atanTable[num];
//				printf("225-270 (%d)= %f, %f\n", num, r, d);
				return 270.0f - d;
			}
		}
		else
		{
			if (-x >= y)
			{
				// 270-315
				r = y / -x;
				num = (SI)(r * atanDevide);
				d = atanTable[num];
//				printf("270-315 (%d)= %f, %f\n", num, r, d);
				return 270.0f + d;
			}
			else
			{
				// 315-360
				r = -x / y;
				num = (SI)(r * atanDevide);
				d = atanTable[num];
//				printf("315-360 (%d)= %f, %f\n", num, r, d);
				return 360.0f - d;
			}
		}
	}
}

SS APL_Sin(SS deg)
{
	while(deg < 0)
	{
		deg += 360;
	}
	while(deg > 360){
		deg -= 360;
	}

	if (deg <= 90)
	{
		return t_Sin256[deg];
	}
	else if (deg <= 180)
	{
		return t_Sin256[180 - deg];
	}
	else if (deg <= 270)
	{
		return -t_Sin256[deg - 180];
	}
	else
	{
		return -t_Sin256[360 - deg];
	}
}

SS APL_Cos(SS deg)
{
	return APL_Sin(90 + deg);
}

SS APL_Tan256(SS deg)
{
	SS cal;
	
	if(APL_Cos(deg) == 0)
	{
		cal = 0xFF;	/* �v�Z�ł��Ȃ����� */
	}
	else
	{
		cal = (APL_Sin(deg) << 8) / APL_Cos(deg);
	}
	return cal;
}

#endif	/* APL_MATH_C */