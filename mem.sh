echo "AES:" | tee mem_usage.txt
arm-none-eabi-size Software/AES/AES.elf | tee -a mem_usage.txt
echo | tee -a mem_usage.txt

echo "ascon128:" | tee -a mem_usage.txt
arm-none-eabi-size Software/ascon128/ascon128.elf | tee -a mem_usage.txt
echo | tee -a mem_usage.txt

echo "ascon128a:" | tee -a mem_usage.txt
arm-none-eabi-size Software/ascon128a/ascon128a.elf | tee -a mem_usage.txt
echo | tee -a mem_usage.txt

echo "elephant160v2:" | tee -a mem_usage.txt
arm-none-eabi-size Software/elephant160v2/elephant160v2.elf | tee -a mem_usage.txt
echo | tee -a mem_usage.txt

echo "giftcofb128v1:" | tee -a mem_usage.txt
arm-none-eabi-size Software/giftcofb128v1/giftcofb128v1.elf | tee -a mem_usage.txt
echo | tee -a mem_usage.txt

echo "grain128aeadv2:" | tee -a mem_usage.txt
arm-none-eabi-size Software/grain128aeadv2/grain128aeadv2.elf | tee -a mem_usage.txt
echo | tee -a mem_usage.txt

echo "isapa128av20:" | tee -a mem_usage.txt
arm-none-eabi-size Software/isapa128av20/isapa128av20.elf | tee -a mem_usage.txt
echo | tee -a mem_usage.txt

echo "isapa128v20:" | tee -a mem_usage.txt
arm-none-eabi-size Software/isapa128v20/isapa128v20.elf | tee -a mem_usage.txt
echo | tee -a mem_usage.txt

echo "photonbeetleaead128rate128v1:" | tee -a mem_usage.txt
arm-none-eabi-size Software/photonbeetleaead128rate128v1/photonbeetleaead128rate128v1.elf | tee -a mem_usage.txt
echo | tee -a mem_usage.txt

echo "romulusn:" | tee -a mem_usage.txt
arm-none-eabi-size Software/romulusn/romulusn.elf | tee -a mem_usage.txt
echo | tee -a mem_usage.txt

echo "schwaemm256128v2:" | tee -a mem_usage.txt
arm-none-eabi-size Software/schwaemm256128v2/schwaemm256128v2.elf | tee -a mem_usage.txt
echo | tee -a mem_usage.txt

echo "schwaemm256256v2:" | tee -a mem_usage.txt
arm-none-eabi-size Software/schwaemm256256v2/schwaemm256256v2.elf | tee -a mem_usage.txt
echo | tee -a mem_usage.txt

echo "tinyjambu:" | tee -a mem_usage.txt
arm-none-eabi-size Software/tinyjambu/tinyjambu.elf | tee -a mem_usage.txt
echo | tee -a mem_usage.txt	

echo "xoodyak:" | tee -a mem_usage.txt
arm-none-eabi-size Software/xoodyak/xoodyak.elf | tee -a mem_usage.txt
echo | tee -a mem_usage.txt


$SHELL #(prevent shell from auto closing)