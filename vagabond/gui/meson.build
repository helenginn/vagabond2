guifiles = [
'AddEntity.cpp',
'AddModel.cpp',
'AddRule.cpp',
'Axes.cpp',
'AxisExplorer.cpp',
'CalculateMetadata.cpp',
'CandidateView.cpp',
'ChainAssignment.cpp',
'ChangeIconOptions.cpp',
'ChooseHeader.cpp',
'ChooseHeaderValue.cpp',
'ChooseEntity.cpp',
'ClusterPointDemo.cpp',
'ClusterView.cpp',
'ConfSpaceView.cpp',
'ColourLegend.cpp',
'ColourScheme.cpp',
'DatasetMenu.cpp',
'Display.cpp',
'DistanceMaker.cpp',
'EntityMenu.cpp',
'EntityFromSequence.cpp',
'FileLine.cpp',
'FileView.cpp',
'FileNavi.cpp',
'GlobalOptions.cpp',
'GuiAtom.cpp',
'GuiBalls.cpp',
'GuiBond.cpp',
'GuiRibbon.cpp',
'GuiDensity.cpp',
'GuiRefls.cpp',
'IconLegend.cpp',
'LineSeries.cpp',
'LineSeriesOptions.cpp',
'MainMenu.cpp',
'MetadataView.cpp',
'ModelMenu.cpp',
'ModelMetadataView.cpp',
'ProgressView.cpp',
'ProjectMenu.cpp',
'RulesMenu.cpp',
'SearchPDB.cpp',
'SerialRefiner.cpp',
'SequenceView.cpp',
'SequenceLine.cpp',
'Toolkit.cpp',
'VagWindow.cpp',
'VaryColourOptions.cpp',
'../cmd/CmdWorker.cpp',
'../cmd/Dictator.cpp',

'AddEntity.h',
'AddModel.h',
'AddRule.h',
'Axes.h',
'CalculateMetadata.h',
'CandidateView.h',
'ChainAssignment.h',
'ChangeIconOptions.h',
'ChooseHeader.h',
'ChooseHeaderValue.h',
'ChooseEntity.h',
'ClusterPointDemo.h',
'ClusterView.h',
'ConfSpaceView.h',
'ColourLegend.h',
'ColourScheme.h',
'DatasetMenu.h',
'Display.h',
'DistanceMaker.h',
'EntityMenu.h',
'EntityFromSequence.h',
'FileLine.h',
'FileView.h',
'GuiAtom.h',
'GuiBond.h',
'GuiDensity.h',
'GuiRefls.h',
'IconLegend.h',
'LineSeries.h',
'LineSeriesOptions.h',
'MainMenu.h',
'MetadataView.h',
'ModelMenu.h',
'ModelMetadataView.h',
'ProgressView.h',
'RulesMenu.h',
'SearchPDB.h',
'SerialRefiner.h',
'SequenceView.h',
'SequenceLine.h',
'Toolkit.h',
'VagWindow.h',
'VaryColourOptions.h',
'../cmd/CmdWorker.h',
'../cmd/Dictator.h',
]

gui = static_library('vaggui', guifiles,
	cpp_args: ['-std=c++11', compile_args],
	link_args: [links],
	dependencies : deps, 
    include_directories: ['../..', '../core'],
	link_with : [core, guielements], 
	install: false)
