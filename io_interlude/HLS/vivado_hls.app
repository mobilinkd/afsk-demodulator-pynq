<project xmlns="com.autoesl.autopilot.project" name="HLS" top="count">
    <includePaths/>
    <libraryPaths/>
    <Simulation>
        <SimFlow name="csim" csimMode="0" lastCsimMode="0"/>
    </Simulation>
    <files xmlns="">
        <file name="../count_test.cpp" sc="0" tb="1" cflags=" -std=c++11 -Wall -Wextra -Wno-unknown-pragmas" blackbox="false"/>
        <file name="HLS/count.hpp" sc="0" tb="false" cflags="" blackbox="false"/>
        <file name="HLS/count.cpp" sc="0" tb="false" cflags="-std=c++11 -Wall -Wextra -Wno-unknown-pragmas" blackbox="false"/>
    </files>
    <solutions xmlns="">
        <solution name="solution1" status="active"/>
    </solutions>
</project>

