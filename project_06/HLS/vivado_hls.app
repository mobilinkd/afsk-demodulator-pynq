<project xmlns="com.autoesl.autopilot.project" top="hdlc_top" name="HLS">
    <includePaths/>
    <libraryPaths/>
    <Simulation>
        <SimFlow name="csim" csimMode="0" lastCsimMode="0"/>
    </Simulation>
    <files xmlns="">
        <file name="../hdlc_test.cpp" sc="0" tb="1" cflags=" -std=c++14 -Wno-unknown-pragmas" blackbox="false"/>
        <file name="HLS/hdlc_top.hpp" sc="0" tb="false" cflags="" blackbox="false"/>
        <file name="HLS/hdlc_top.cpp" sc="0" tb="false" cflags="-std=c++11" blackbox="false"/>
        <file name="HLS/hdlc.hpp" sc="0" tb="false" cflags="" blackbox="false"/>
        <file name="HLS/crc.hpp" sc="0" tb="false" cflags="" blackbox="false"/>
    </files>
    <solutions xmlns="">
        <solution name="solution1" status="active"/>
    </solutions>
</project>

