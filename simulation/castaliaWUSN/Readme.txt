# ****************************************************************************
# *  Copyright: National ICT Australia,  2007 - 2010                         *
# *  Developed at the ATP lab, Networked Systems research theme              *
# *  Author(s): Athanassios Boulis, Dimosthenis Pediaditakis                 *
# *  This file is distributed under the terms in the attached LICENSE file.  *
# *  If you do not find this file, copies can be found by writing to:        *
# *                                                                          *
# *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
# *      Attention:  License Inquiry.                                        *
# *                                                                          *
# ***************************************************************************/


TO INSTALL Castalia type:
> make makefiles
> make

READ the Instalation guide for more details.

================================================================================
In the Root directory there are the following contents :

bin           --> Dir containing the Castalia and CastaliaResults scripts

doc           --> Directory containing installation manual and user guide.

out           --> Dir produced by the build process. Contains all interediate
                  built files, as well as the Castalia executable.

Simulations   --> Dir containing various simulation configuration files
                  Simulations can easily be run from its sub dirs.

src           --> The source code of Castalia (*.cc, *.h, *.msg, *.ned files)
                  The directory's structure corresponds to the structure of the 
                  compound modules and submodules that Castalia has.

src/Makefile  --> File created after the invocation of the makemake script.

makemake      --> The script to generate the appropriate Makefiles. It uses
                  OMNeT's opp_makemake tool.

src/CastaliaBin --> The Castalia executable. (or CataliaBin_dbg for the debug version)

CHANGES.txt   --> The change log.

LICENSE       --> The License in plain text

VERSION       --> File with the current version/build info

===============  READ THE INSTALATION GUIDE AND THE USER'S MANUAL  ==============
