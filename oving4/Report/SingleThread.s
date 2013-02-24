cvtsi2sd  -44(%rbp), %xmm0  #XMM0 = j
movsd     .LC1(%rip), %xmm1 #XMM1 = 1.
movapd    %xmm1, %xmm2      #XMM2 = XMM1
divsd     %xmm0, %xmm2      #XMM2 = XMM2/XMM0
movapd    %xmm2, %xmm0      #XMM0 = XMM2
cvtsi2sd  -44(%rbp), %xmm1  #XMM1 = j
divsd     %xmm1, %xmm0      #XMM0 = XMM0/XMM1
movsd     %xmm0, (%rax)     #XMM0 -> Result