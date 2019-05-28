<project xmlns="com.autoesl.autopilot.project" name="HLS" top="demodulate_top">
    <includePaths/>
    <libraryPaths/>
    <Simulation>
        <SimFlow name="csim" csimMode="0" lastCsimMode="0"/>
    </Simulation>
    <files xmlns="">
        <file name="../demodulate_test.cpp" sc="0" tb="1" cflags=" -std=c++11 -Wno-unknown-pragmas" blackbox="false"/>
        <file name="HLS/hysteresis.hpp" sc="0" tb="false" cflags="" blackbox="false"/>
        <file name="HLS/fir_filter.hpp" sc="0" tb="false" cflags="" blackbox="false"/>
        <file name="HLS/digital_pll.hpp" sc="0" tb="false" cflags="" blackbox="false"/>
        <file name="HLS/demodulate_top.hpp" sc="0" tb="false" cflags="" blackbox="false"/>
        <file name="HLS/demodulate_top.cpp" sc="0" tb="false" cflags="-std=c++11 -Wno-unknown-pragmas" blackbox="false"/>
    </files>
    <solutions xmlns="">
        <solution name="solution1" status="active"/>
        <solution name="solution1" status="active"/>
    </solutions>
</project>

