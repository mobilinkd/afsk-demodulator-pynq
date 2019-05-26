<project xmlns="com.autoesl.autopilot.project" name="HLS" top="demodulate">
    <includePaths/>
    <libraryPaths/>
    <Simulation>
        <SimFlow name="csim" csimMode="0" lastCsimMode="0"/>
    </Simulation>
    <files xmlns="">
        <file name="../demodulate_test.cpp" sc="0" tb="1" cflags=" -std=c++11 -Wall -Wextra -Wno-unused-label -isystem/data/opt/Xilinx/Vivado/2018.3/include -Wno-unknown-pragmas" blackbox="false"/>
        <file name="HLS/demodulate.hpp" sc="0" tb="false" cflags="" blackbox="false"/>
        <file name="HLS/demodulate.cpp" sc="0" tb="false" cflags="-std=c++11 -Wall -Wextra -Wno-unused-label -isystem/data/opt/Xilinx/Vivado/2018.3/include -Wno-unknown-pragmas" blackbox="false"/>
    </files>
    <solutions xmlns="">
        <solution name="solution1" status="active"/>
    </solutions>
</project>

