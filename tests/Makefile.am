TESTS = runtests
check_PROGRAMS = $(TESTS)

runtests_SOURCES =  testmain.cpp\
					$(NULL)\
                    ../src/FileClasses/INIFile.cpp\
                    $(NULL)\
                    INIFileTestCase/INIFileTestCase1.cpp\
                    INIFileTestCase/INIFileTestCase2.cpp\
                    $(NULL)

EXTRA_DIST = INIFileTestCase/INIFileTestCase1.h\
             INIFileTestCase/INIFileTestCase2.h\
             INIFileTestCase/INIFileTestCase1.ini\
             $(NULL)



runtests_CXXFLAGS = $(CPPUNIT_CFLAGS) -DTESTSRC=\"$(srcdir)\" -I$(top_srcdir)/include
runtests_LDFLAGS = $(CPPUNIT_LIBS) -lcppunit
