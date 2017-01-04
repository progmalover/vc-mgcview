// Graph.cpp : implementation file
//

#include "stdafx.h"
//#include "MyDraw.h"
#include "Graph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraph

#ifndef _OLD_CLIENT_CHART_VER_

CGraph::CGraph()
{
	graphSeries = new CObList();
	tickRange = 100;
	seriesSize = 1;//0

	graphHasLegend = FALSE;
	legendWidth = 0;

	graphType = 0;

	m_scale = 1;

	dataColor[0] = RGB(0,255,0);  //green
	dataColor[1] = RGB(0,0,255);  //blue
	dataColor[2] = RGB(255,0,0);  //red
	dataColor[3] = RGB(255,255,0);  //yellow
	dataColor[4] = RGB(255,153,51);  //orange
	dataColor[5] = RGB(255,51,153);  //hot pink
	dataColor[6] = RGB(153,0,204);  //purple
	dataColor[7] = RGB(0,255,255);  //cyan
	dataColor[8] = RGB(0,0,0);  //black
	dataColor[9] = RGB(255,255,255);  //white

	xAxisAlign = 0;  //horizontal
	xAxisLabelLength = 0;

	//added by LiuSong
	CurColorIndex = 0;
}

CGraph::CGraph(int type)
{
	graphSeries = new CObList();
	tickRange = 100;
	seriesSize = 1;//0

	graphHasLegend = FALSE;
	legendWidth = 0;

	graphType = type;

	dataColor[0] = RGB(0,255,0);  //green
	dataColor[1] = RGB(0,0,255);  //blue
	dataColor[2] = RGB(255,0,0);  //red
	dataColor[3] = RGB(255,255,0);  //yellow
	dataColor[4] = RGB(255,153,51);  //orange
	dataColor[5] = RGB(255,51,153);  //hot pink
	dataColor[6] = RGB(153,0,204);  //purple
	dataColor[7] = RGB(0,255,255);  //cyan
	dataColor[8] = RGB(0,0,0);  //black
	dataColor[9] = RGB(255,255,255);  //white

	xAxisAlign = 0;  //horizontal
	xAxisLabelLength = 0;

	//added by LiuSong
	CurColorIndex = 0;
}

CGraph::~CGraph()
{
	
	POSITION pos;
	pos = graphSeries->GetHeadPosition();
	CGraphSeries* tmpSeries;

	for(int x = 1; x <= graphSeries->GetCount(); x++)
	{
		tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
		delete tmpSeries;
	}
	//graphSeries->RemoveAll();

	delete graphSeries;
}


BEGIN_MESSAGE_MAP(CGraph, CStatic)
	//{{AFX_MSG_MAP(CGraph)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

//added by LiuSong
void CGraph::SetScale(int scale)
{
	m_scale = scale;
}

int CGraph::GetScale()
{
	return m_scale;	
}

void CGraph::SetCurColorIndex(int idx)
{
	if(idx > 0 && idx < 10)
	{
		CurColorIndex = idx;
	}
}

int CGraph::GetCurColorIndex()
{
	return CurColorIndex;	
}


void CGraph::SetXTickSpace(int xDist)
{
	XAxisTickSpace = xDist;
}

void CGraph::SetXTickRange(int maxTick)
{
	XAxisTickRange = maxTick;
}

// CGraph message handlers
void CGraph::SetTickSpace(int yDist)
{
	tickSpace = yDist;
}

void CGraph::SetTickRange(int maxTick)
{
	tickRange = maxTick;
}

void CGraph::SetGraphType(int gType)
{
	graphType = gType;
}

void CGraph::SetXAxisAlignment(int alignValue)
{
	xAxisAlign=alignValue;
}

int CGraph::GetXAxisAlignment()
{
	return xAxisAlign;
}

void CGraph::DrawGraph(CDC *pDC,int StartX,int StartY,int nWidth,int nHeight)
{
	CString tickLabel;

	maxHeight = nHeight - 20;
	maxWidth = nWidth - 20;

	//draw graph title
	pDC->TextOut((maxWidth / 2) - ((graphTitle.GetLength() * 16) / 2),10 + StartY, graphTitle);

	//if(0x01 == graphType)  //line
	{
		//X-axis will be raised by 20 pixels for tick-labels
		//Y-axis will be shifted by (max(tick-label size) * 10) + label * 10
		tickLabel.Format(_T("%d"), tickRange);

		//determine axis specifications 
		//tick label offset is 4 (for the tick) + 6 (total 10) from axis line

		xApexPoint = (tickLabel.GetLength() * 10);

		//y apex based on 5 + 15 (x label) + 4 (for the tick) + 4 (text below tick) + 12 (tick label) + 10
		if(!xAxisAlign)  //horizontal
		{
			yApexPoint = (maxHeight - 5) - 45;		//apex points are the cross section of axis lines
		}
		else
		{
			yApexPoint = (maxHeight - 5) - (xAxisLabelLength * 12) - 10;
		}

		yAxisHeight = yApexPoint - 40;
		xAxisWidth = (maxWidth - 5) - xApexPoint;

		//add by LiuSong
		xApexPoint += StartX;
		yApexPoint += StartY;
	}

	//draw legend
	if(graphHasLegend)
	{
		DrawLegend(pDC);
	}

	//if(0x01 == graphType)  //line
	{
		//draw axis lines
		DrawAxis(pDC);
	}

	//draw series data and labels
	DrawSeries(pDC);
}

void CGraph::DrawVertDashLine(CDC *pDC,int x, int y, int height)
{
	COLORREF color = RGB(127,127,127);
	for(int ii = y; ii < y + height ;)
	{
		pDC->SetPixel(x,ii,color);
		ii+=2;
	}
}

void CGraph::DrawHoriDashLine(CDC *pDC,int x, int y, int width)
{
	COLORREF color = RGB(127,127,127);
	for(int ii = x ; ii < x + width ;)
	{
		pDC->SetPixel(ii,y,color);
		ii+=2;
	}
}

void CGraph::DrawAxis(CDC *pDC)
{
	pDC->FillSolidRect(xApexPoint,yApexPoint - yAxisHeight, xAxisWidth, yAxisHeight, RGB(255,255,255));

	pDC->SetTextColor(RGB(0,0,0));

	//draw y axis
	pDC->MoveTo(xApexPoint, yApexPoint);  
	pDC->LineTo(xApexPoint, yApexPoint - yAxisHeight);

	//draw x axis
	pDC->MoveTo(xApexPoint, yApexPoint);  
	
	if(graphHasLegend)
	{
		pDC->LineTo(xApexPoint + (xAxisWidth - legendWidth - 10), yApexPoint);
	}
	else
	{
		pDC->LineTo(xApexPoint + xAxisWidth, yApexPoint);
	}

	//draw labels

	if (axisYLabel == _T("\x2103"))
	{
		CFont oldFont, newFont;
		LOGFONT lf;
		memset(&lf, 0x00, sizeof(lf));
		lf.lfHeight = 16;
		lf.lfWeight = 700;
		_tcscpy(lf.lfFaceName, _T("Lucida Sans Unicode"));

		newFont.CreateFontIndirect(&lf);

		oldFont.Attach(pDC->SelectObject(newFont));

		pDC->TextOut(xApexPoint+4, yApexPoint-24-yAxisHeight, axisYLabel);

		pDC->SelectObject(oldFont.Detach());
	}
	else
	{
		pDC->TextOut(xApexPoint+4, yApexPoint-24-yAxisHeight, axisYLabel);
	}

	//pDC->TextOut(10, yApexPoint - (maxHeight / 2) - 6, axisYLabel);


	pDC->TextOut(xApexPoint + xAxisWidth  - (axisXLabel.GetLength() * 19), yApexPoint - 16 , axisXLabel);
	//pDC->TextOut(xApexPoint + xAxisWidth , yApexPoint, axisXLabel);

	//to allow scalability (height may be less than tickRange)
	double tickScale = 0.00;

	tickScale = ((yAxisHeight * 1.00) / (tickRange * 1.00)) * tickSpace;


	//draw y axis ticks
	for(int y = 1; y <= tickRange / tickSpace; y++)  //no tick at 0
	{
		int tickYLocation = yApexPoint - (y * tickScale);

		pDC->MoveTo(xApexPoint , tickYLocation);
		pDC->LineTo(xApexPoint + 4, tickYLocation);

		DrawHoriDashLine(pDC, xApexPoint + 4 , tickYLocation, xAxisWidth-4);


		//draw tick label
		CString tickLabel;
		tickLabel.Format(_T("%d"), y * tickSpace);

		pDC->TextOut(xApexPoint - (tickLabel.GetLength() * 10), tickYLocation - 6, tickLabel);
	}

	//draw X axis tick marks
	POSITION pos = graphSeries->GetHeadPosition();
	int iGraphSeriesCount = graphSeries->GetCount() ;
	CGraphSeries* tmpSeries;
	
	float seriesSpace;
	if(graphHasLegend)
	{
		seriesSpace= (xAxisWidth - legendWidth - 10)  / iGraphSeriesCount;
	}
	else
	{	
		//modified by LiuSong
		seriesSpace = 1.00;
		seriesSpace	*= xAxisWidth;
		seriesSpace	/= iGraphSeriesCount;
	}

	int nStep = 1;
	if(iGraphSeriesCount <= 15)
	{
		nStep = 1;
	}
	else if(iGraphSeriesCount <= 30)
	{
		nStep = 2;
	}
	else if(iGraphSeriesCount < 60)
	{
		nStep = 3;
	}
	else //if(iGraphSeriesCount < 90)
	{
		nStep = 7;
	}
	
	
	
	float fLen = 0.0;
	//start to display data 
	for(int nDataCount = 0; nDataCount < (iGraphSeriesCount); nDataCount++)
	{
		tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

		int tickXLocation;
		/* modified by LiuSong
		tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));
		*/
		tickXLocation = xApexPoint;
		tickXLocation += (int)fLen;
		fLen += seriesSpace;

		pDC->MoveTo(tickXLocation,yApexPoint - 4);
		pDC->LineTo(tickXLocation,yApexPoint);

		if(nDataCount%nStep)
		{
			continue;
		}

		DrawVertDashLine(pDC, tickXLocation, yApexPoint - yAxisHeight , yAxisHeight-4);
		//draw tick label
		CString tickLabel;
		tickLabel = tmpSeries->GetLabel();
		if(!xAxisAlign)  //horizontal
		{
			pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * 8) / 2), yApexPoint + 8, tickLabel);
		}
		else
		{
			CFont sideFont2;
			sideFont2.CreateFont(12, 0, (xAxisAlign * 10), 0, 700, TRUE, FALSE, 0,
				ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,LPCTSTR("Arial"));
			CFont *ptOldFont = (CFont*) pDC->SelectObject(&sideFont2);
			if(xAxisAlign < 180)
			{	
				pDC->TextOut(tickXLocation - 8, yApexPoint + 8 + (xAxisLabelLength * 8), tickLabel);
			}
			else
			{
				pDC->TextOut(tickXLocation + 8, yApexPoint + 8, tickLabel);
			}
			pDC->SelectObject(ptOldFont);
		}
	}
#if 0 
	CString tickLabel;
	tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
	tickLabel = tmpSeries->GetLabel();
	if(!xAxisAlign)  //horizontal
	{
		pDC->TextOut(xApexPoint + xAxisWidth - ((tickLabel.GetLength() * 8) / 2), yApexPoint + 8, tickLabel);
	}
	else
	{
		CFont sideFont2;
		sideFont2.CreateFont(12, 0, (xAxisAlign * 10), 0, 700, TRUE, FALSE, 0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,LPCTSTR("Arial"));
		CFont *ptOldFont = (CFont*) pDC->SelectObject(&sideFont2);
		if(xAxisAlign < 180)
		{	
			pDC->TextOut(xApexPoint + xAxisWidth - 8, yApexPoint + 8 + (xAxisLabelLength * 8), tickLabel);
		}
		else
		{
			pDC->TextOut(xApexPoint + xAxisWidth + 8, yApexPoint + 8, tickLabel);
		}
		pDC->SelectObject(ptOldFont);
	}
#endif 
	DrawVertDashLine(pDC, xApexPoint + xAxisWidth - 1, yApexPoint - yAxisHeight , yAxisHeight);
}

#if 0
void CGraph::AddSeries(CGraphSeries *dataSet)
{
	int numData = 0;
	for(int i = 0; i < 10; i++)
	{
		if(dataSet->GetData(i) > 0)
			numData++;
		if(dataSet->GetLabel().GetLength() > xAxisLabelLength)
			xAxisLabelLength = dataSet->GetLabel().GetLength();
	}
	if(numData > seriesSize)
		seriesSize = numData;

	graphSeries->AddTail(dataSet);
}
#else
void CGraph::AddSeries(CGraphSeries *dataSet)
{
	if(dataSet->GetLabel().GetLength() > xAxisLabelLength)
			xAxisLabelLength = dataSet->GetLabel().GetLength();
	graphSeries->AddTail(dataSet);
}
#endif

void CGraph::SetXAxisLabel(CString label)
{
	axisXLabel = label;
}

void CGraph::SetYAxisLabel(CString label)
{
	axisYLabel=label;
}

void CGraph::SetColor(int group,COLORREF groupColor)
{
	dataColor[group] = groupColor;
}

COLORREF CGraph::GetColor(int group)
{
	return dataColor[group];
}

void CGraph::DrawSeries(CDC *pDC)
{
	int barWidth;
	int dataPlotSize;   //used to plot rects of data
	int barL, barT, barR, barB;
	int tickXLocation;
	float seriesSpace;
	double barHeight;  //for scalability
	POSITION pos;
	CGraphSeries* tmpSeries;

	int iGraphSeriesCount = graphSeries->GetCount();
	int lastXLoc, lastYLoc;
	for(int s = 0; s < seriesSize; s++)
	{
		//determine width of barchart data blocks
		if(graphHasLegend)
		{	
			//modified by LiuSong
			//seriesSpace = (xAxisWidth - legendWidth - 10) / iGraphSeriesCount;
			seriesSpace = (xAxisWidth - legendWidth - 10) / iGraphSeriesCount ;
		}
		else
		{	
			//modified by LiuSong
			//seriesSpace = xAxisWidth / graphSeries->GetCount();
			seriesSpace = 1.00;
			seriesSpace	*= xAxisWidth;
			seriesSpace	/= (iGraphSeriesCount*10);
		}

		barWidth = (seriesSpace * .6) / seriesSize;
		dataPlotSize = seriesSize * barWidth;

		pos = graphSeries->GetHeadPosition();

		lastXLoc = -1;
		lastYLoc = -1;
		float fLen = seriesSpace;//0.0;
		for(int nDataCount = 0; nDataCount < (iGraphSeriesCount ); nDataCount++)
		{
			tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

			
			for(int tt = 0; tt < 10 ; tt ++)
			{
				/* modified by LiuSong
				tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));
				*/
				tickXLocation = xApexPoint;
				tickXLocation += (int)fLen;

				fLen += seriesSpace;

				barHeight = 0.00;
				//to allow scalability (height may be less than tickRange)
				double dataScale = 0.00;
				dataScale = (yAxisHeight * 1.00) / (tickRange * 1.00);

				int ndataVal =  tmpSeries->GetData(tt);
				if(ndataVal < 0 ) 
				{	
					continue;
				}
				barHeight = (ndataVal * 1.00) * dataScale;

				int yDataLocation = yApexPoint - barHeight;
				
				//now have x and y location of center of ellipse
				COLORREF barColor;
				//modified by LiuSong
				//barColor = GetColor(s);
				barColor = GetColor(GetCurColorIndex());

				CBrush brush (barColor);
				CBrush* pOldBrush;
				pOldBrush = pDC->SelectObject(&brush);

				//draw line back to last data member
				//if(nDataCount > 0 || tt > 0)
				if(lastXLoc != -1 && lastYLoc != -1 )
				{
					CPen* pOldPen;
					CPen linePen (PS_SOLID, 2, barColor);
					pOldPen = pDC->SelectObject(&linePen);
#if 0
					pDC->MoveTo(lastXLoc + 2, lastYLoc - 1);
					pDC->LineTo(tickXLocation - 3, yDataLocation - 1);
#else
					pDC->MoveTo(lastXLoc, lastYLoc);
					pDC->LineTo(tickXLocation , yDataLocation );
#endif
					pDC->SelectObject(pOldPen);
				}
				
#if 0 
				//now draw ellipse...
				pDC->Ellipse(tickXLocation - 3, yDataLocation - 3,tickXLocation + 3, yDataLocation + 3);
#else
				pDC->Ellipse(tickXLocation - 2, yDataLocation - 2,tickXLocation + 2, yDataLocation + 2);
#endif
				lastXLoc = tickXLocation;
				lastYLoc = yDataLocation;
				pDC->SelectObject(&pOldBrush);
			}

		}
	}
}

void CGraph::SetLegend(int datagroup,CString label)
{
	/* disabled by LiuSong
	graphLegend.SetLegendText(datagroup, label);
	graphHasLegend = TRUE;
	*/
}

void CGraph::DrawLegend(CDC *pDC)
{
	//determine size of legend
	//12 chars per seriesSize + 6 for spacing (3 top and bottom) 
	//+ 1 set for label title(3+12+6) + rectangle (2 chars) + 3 for final bottom buffer
	int legendHeight = 23 + (seriesSize * 18) + 3;
	int legendL, legendT, legendR, legendB;
	int barL, barT, barR, barB;

	legendT = (maxHeight / 2) - (legendHeight / 2);
	legendB = legendT + legendHeight;
	legendR = maxWidth - 5;
	legendL = legendR - ((graphLegend.GetLegendLength() * 12) + 50);
		//allows 50 pixels for display of legend bar 45 + 5 space.
	legendWidth = legendR - legendL;

	pDC->Rectangle(legendL, legendT, legendR, legendB);
	pDC->TextOut(legendL + (legendWidth / 2) - 24, legendT + 3, Translate(_T("Legend")));
	for(int i = 0; i < seriesSize; i++)
	{
		//top "Legend" text will use 12 + 3 top + 6 bottom (21 total)
		//each legend label will need 3 chars on top, so the 24 in the offset
		//each label than uses 12 + 3 below + 3 above next label, so 18
		// in the i * offset.  
		pDC->TextOut(legendL + 5, legendT + 24 + (i * 18), graphLegend.GetLegendText(i));

		//draw bar
		COLORREF barColor;
		barColor = GetColor(i);
		CBrush brush (barColor);
		CBrush* pOldBrush;
		pOldBrush = pDC->SelectObject(&brush);

		barL = legendL + 5 + (graphLegend.GetLegendText(i).GetLength() * 8) + 5;
		barT = legendT + 24 + (i * 18) + 1, graphLegend.GetLegendText(i);
		barR = legendR - 5;
		barB = barT + 12;
		pDC->Rectangle(barL, barT, barR, barB);

		pDC->SelectObject(&pOldBrush);
	}
}

void CGraph::SetGraphTitle(CString title)
{
	graphTitle = title;
}

#else

CGraph::CGraph()
{
	graphSeries = new CObList();
	tickRange = 100;
	seriesSize = 1;//0

	graphHasLegend = FALSE;
	legendWidth = 0;

	graphType = 0;

	m_scale = 1;

	dataColor[0] = RGB(0,255,0);  //green
	dataColor[1] = RGB(0,0,255);  //blue
	dataColor[2] = RGB(255,0,0);  //red
	dataColor[3] = RGB(255,255,0);  //yellow
	dataColor[4] = RGB(255,153,51);  //orange
	dataColor[5] = RGB(255,51,153);  //hot pink
	dataColor[6] = RGB(153,0,204);  //purple
	dataColor[7] = RGB(0,255,255);  //cyan
	dataColor[8] = RGB(0,0,0);  //black
	dataColor[9] = RGB(255,255,255);  //white

	xAxisAlign = 0;  //horizontal
	xAxisLabelLength = 0;

	//added by LiuSong
	CurColorIndex = 0;
}

CGraph::CGraph(int type)
{
	graphSeries = new CObList();
	tickRange = 100;
	seriesSize = 1;//0

	graphHasLegend = FALSE;
	legendWidth = 0;

	graphType = type;

	dataColor[0] = RGB(0,255,0);  //green
	dataColor[1] = RGB(0,0,255);  //blue
	dataColor[2] = RGB(255,0,0);  //red
	dataColor[3] = RGB(255,255,0);  //yellow
	dataColor[4] = RGB(255,153,51);  //orange
	dataColor[5] = RGB(255,51,153);  //hot pink
	dataColor[6] = RGB(153,0,204);  //purple
	dataColor[7] = RGB(0,255,255);  //cyan
	dataColor[8] = RGB(0,0,0);  //black
	dataColor[9] = RGB(255,255,255);  //white

	xAxisAlign = 0;  //horizontal
	xAxisLabelLength = 0;

	//added by LiuSong
	CurColorIndex = 0;
}

CGraph::~CGraph()
{
	
	POSITION pos;
	pos = graphSeries->GetHeadPosition();
	CGraphSeries* tmpSeries;

	for(int x = 1; x <= graphSeries->GetCount(); x++)
	{
		tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
		delete tmpSeries;
	}
	//graphSeries->RemoveAll();

	delete graphSeries;
}


BEGIN_MESSAGE_MAP(CGraph, CStatic)
	//{{AFX_MSG_MAP(CGraph)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

//added by LiuSong
void CGraph::SetScale(int scale)
{
	m_scale = scale;
}

int CGraph::GetScale()
{
	return m_scale;	
}

void CGraph::SetCurColorIndex(int idx)
{
	if(idx > 0 && idx < 10)
	{
		CurColorIndex = idx;
	}
}

int CGraph::GetCurColorIndex()
{
	return CurColorIndex;	
}

// CGraph message handlers
void CGraph::SetTickSpace(int yDist)
{
	tickSpace = yDist;
}

void CGraph::SetTickRange(int maxTick)
{
	tickRange = maxTick;
}

void CGraph::SetGraphType(int gType)
{
	graphType = gType;
}

void CGraph::SetXAxisAlignment(int alignValue)
{
	xAxisAlign=alignValue;
}

int CGraph::GetXAxisAlignment()
{
	return xAxisAlign;
}

void CGraph::DrawGraph(CDC *pDC,int StartX,int StartY,int nWidth,int nHeight)
{
	CString tickLabel;
#if 0	 // modified by LiuSong
	CWnd* graphWnd = pDC->GetWindow();
	CRect graphRect;
	graphWnd->GetClientRect(&graphRect);

	maxHeight = graphRect.Height() - 20;  //for frame window and status bar
	maxWidth = graphRect.Width() - 5;  //for frame window
	//We will leave 5 pixels blank on all sides of the graph.  So
	//top-left side of graph is at 5,5 and the bottom-right side of
	//graph is at ((maxHeight - 5), (maxWidth - 5))
	//these settings are altered by axis labels and legends.
#else
	maxHeight = nHeight -20;
	maxWidth = nWidth -5;
#endif
	//draw graph title
#if 0 //modified by LiuSong
	CFont titleFont;
	titleFont.CreateFont(28, 0, 0, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,LPCTSTR("Arial"));
	CFont* pOldFont = (CFont*) pDC->SelectObject(&titleFont);

	pDC->TextOut((maxWidth / 2) - ((graphTitle.GetLength() * 16) / 2),10 + StartY, graphTitle);
	pDC->SelectObject(pOldFont);
#else
	pDC->TextOut((maxWidth / 2) - ((graphTitle.GetLength() * 16) / 2),10 + StartY, graphTitle);
#endif

	//if(0x01 == graphType)  //line
	{
		//X-axis will be raised by 20 pixels for tick-labels
		//Y-axis will be shifted by (max(tick-label size) * 10) + label * 10
		tickLabel.Format(_T("%d"), tickRange);

		//determine axis specifications 
		//tick label offset is 4 (for the tick) + 6 (total 10) from axis line
#if 0   // modified by LiuSong
		xApexPoint = 5 + (tickLabel.GetLength() * 8) + 10 + 30/*(axisYLabel.GetLength() * 8)*/ + 5; //allowing 10 pixels per char in tick label
#else
		xApexPoint = (tickLabel.GetLength() * 10);
#endif

		//y apex based on 5 + 15 (x label) + 4 (for the tick) + 4 (text below tick) + 12 (tick label) + 10
		if(!xAxisAlign)  //horizontal
		{
			yApexPoint = (maxHeight - 5) - 45;		//apex points are the cross section of axis lines
		}
		else
		{
			yApexPoint = (maxHeight - 5) - (xAxisLabelLength * 12) - 10;
		}

		yAxisHeight = yApexPoint - 40;
		xAxisWidth = (maxWidth - 5) - xApexPoint;

		//add by LiuSong
		xApexPoint += StartX;
		yApexPoint += StartY;
	}

	//draw legend
	if(graphHasLegend)
	{
		DrawLegend(pDC);
	}

	//if(0x01 == graphType)  //line
	{
		//draw axis lines
		DrawAxis(pDC);
	}

	//draw series data and labels
	DrawSeries(pDC);
}

void CGraph::DrawVertDashLine(CDC *pDC,int x, int y, int height)
{
	COLORREF color = RGB(127,127,127);
	for(int ii = y; ii < y + height ;)
	{
		pDC->SetPixel(x,ii,color);
		ii+=2;
	}
}

void CGraph::DrawHoriDashLine(CDC *pDC,int x, int y, int width)
{
	COLORREF color = RGB(127,127,127);
	for(int ii = x ; ii < x + width ;)
	{
		pDC->SetPixel(ii,y,color);
		ii+=2;
	}
}

void CGraph::DrawAxis(CDC *pDC)
{
	pDC->FillSolidRect(xApexPoint,yApexPoint - yAxisHeight, xAxisWidth, yAxisHeight, RGB(255,255,255));

	pDC->SetTextColor(RGB(0,0,0));

	//draw y axis
	pDC->MoveTo(xApexPoint, yApexPoint);  
	pDC->LineTo(xApexPoint, yApexPoint - yAxisHeight);

	//draw x axis
	pDC->MoveTo(xApexPoint, yApexPoint);  
	
	if(graphHasLegend)
	{
		pDC->LineTo(xApexPoint + (xAxisWidth - legendWidth - 10), yApexPoint);
	}
	else
	{
		pDC->LineTo(xApexPoint + xAxisWidth, yApexPoint);
	}

	//draw labels
#if 0 //modified by LiuSong
	CFont sideFont;
	sideFont.CreateFont(16, 0, 900, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,LPCTSTR("Arial"));
	CFont* pOldFont = (CFont*) pDC->SelectObject(&sideFont);

	pDC->TextOut(10, yApexPoint + (maxHeight / 2) - 6, axisYLabel);
	//pDC->TextOut(xApexPoint-10, yApexPoint-10-yAxisHeight, axisYLabel);

	pDC->SelectObject(pOldFont);
#else
	if (axisYLabel == _T("\x2103"))
	{
		CFont oldFont, newFont;
		LOGFONT lf;
		memset(&lf, 0x00, sizeof(lf));
		lf.lfHeight = 16;
		lf.lfWeight = 700;
		_tcscpy(lf.lfFaceName, _T("Lucida Sans Unicode"));

		newFont.CreateFontIndirect(&lf);

		oldFont.Attach(pDC->SelectObject(newFont));

		pDC->TextOut(xApexPoint+4, yApexPoint-24-yAxisHeight, axisYLabel);

		pDC->SelectObject(oldFont.Detach());
	}
	else
	{
		pDC->TextOut(xApexPoint+4, yApexPoint-24-yAxisHeight, axisYLabel);
	}

	//pDC->TextOut(10, yApexPoint - (maxHeight / 2) - 6, axisYLabel);
	
#endif

	pDC->TextOut(xApexPoint + xAxisWidth  - (axisXLabel.GetLength() * 19), yApexPoint - 16 , axisXLabel);
	//pDC->TextOut(xApexPoint + xAxisWidth , yApexPoint, axisXLabel);

	//to allow scalability (height may be less than tickRange)
	double tickScale = 0.00;
#if 0
	if(tickRange > yAxisHeight)
	{
		tickScale = ((yAxisHeight * 1.00) / (tickRange * 1.00)) * tickSpace;
	}
	else 
	{	
		tickScale = tickSpace;
	}
#else
	tickScale = ((yAxisHeight * 1.00) / (tickRange * 1.00)) * tickSpace;
#endif

	//draw y axis ticks
	for(int y = 1; y <= tickRange / tickSpace; y++)  //no tick at 0
	{
		int tickYLocation = yApexPoint - (y * tickScale);

#if 0	//modified by LiuSong
		pDC->MoveTo(xApexPoint - 4, tickYLocation);
#else
		pDC->MoveTo(xApexPoint , tickYLocation);
#endif
		pDC->LineTo(xApexPoint + 4, tickYLocation);

		DrawHoriDashLine(pDC, xApexPoint + 4 , tickYLocation, xAxisWidth-4);


		//draw tick label
		CString tickLabel;
		tickLabel.Format(_T("%d"), y * tickSpace);

		pDC->TextOut(xApexPoint - (tickLabel.GetLength() * 10), tickYLocation - 6, tickLabel);
	}

	for(int xx = 1;  ; xx++)
	{
		int tickXLocation = xApexPoint + (xx * tickScale);
		if(tickXLocation > xApexPoint + xAxisWidth)
		{
			break;
		}
		DrawVertDashLine(pDC, tickXLocation, yApexPoint - yAxisHeight , yAxisHeight-4);
	}

	//draw X axis tick marks
	POSITION pos = graphSeries->GetHeadPosition();
	int iGraphSeriesCount = graphSeries->GetCount();
	CGraphSeries* tmpSeries;
	
	float seriesSpace;
	if(graphHasLegend)
	{
		//modified by LiuSong
		//seriesSpace= (xAxisWidth - legendWidth - 10) / graphSeries->GetCount();
		seriesSpace= (xAxisWidth - legendWidth - 10) * m_scale / iGraphSeriesCount;
	}
	else
	{	
		//modified by LiuSong
		seriesSpace = 1.00;
		seriesSpace	*= xAxisWidth;
		seriesSpace *= m_scale;
		seriesSpace	/= iGraphSeriesCount;
	}
	
	//int step = graphSeries->GetCount() *16 / m_scale /xAxisWidth;  
	int xLabelStep = (16 / seriesSpace);
	int xAxisStep =  (4 / seriesSpace);
	int nStartx = iGraphSeriesCount - (iGraphSeriesCount / m_scale);
	
	for(int ii = 1; ii < nStartx ;ii++)
	{
		tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
	}

	//start to display data 
	for(int nDataCount = 1; nDataCount <= (iGraphSeriesCount/m_scale); nDataCount++)
	{
		tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

		int tickXLocation;
		/* modified by LiuSong
		tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));
		*/
		float fLen = (nDataCount * seriesSpace) - (seriesSpace/2);
		tickXLocation = xApexPoint;
		tickXLocation += (int)fLen;

		if(xLabelStep < 2 || !(nDataCount % xLabelStep) )
		{
			pDC->MoveTo(tickXLocation,yApexPoint - 4);
			pDC->LineTo(tickXLocation,yApexPoint);

			//draw tick label
			CString tickLabel;
			tickLabel = tmpSeries->GetLabel();
			if(!xAxisAlign)  //horizontal
			{
				pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * 8) / 2), yApexPoint + 8, tickLabel);
			}
			else
			{
				CFont sideFont2;
				sideFont2.CreateFont(12, 0, (xAxisAlign * 10), 0, 700, TRUE, FALSE, 0,
					ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,LPCTSTR("Arial"));
				CFont *ptOldFont = (CFont*) pDC->SelectObject(&sideFont2);
				if(xAxisAlign < 180)
				{	
					pDC->TextOut(tickXLocation - 8, yApexPoint + 8 + (xAxisLabelLength * 8), tickLabel);
				}
				else
				{
					pDC->TextOut(tickXLocation + 8, yApexPoint + 8, tickLabel);
				}
				pDC->SelectObject(ptOldFont);
			}
		}
		else if(xAxisStep < 2 || !(nDataCount % xAxisStep) )
		{
			pDC->MoveTo(tickXLocation,yApexPoint - 4);
			pDC->LineTo(tickXLocation,yApexPoint);
		}
	}
}

#if 0
void CGraph::AddSeries(CGraphSeries *dataSet)
{
	int numData = 0;
	for(int i = 0; i < 10; i++)
	{
		if(dataSet->GetData(i) > 0)
			numData++;
		if(dataSet->GetLabel().GetLength() > xAxisLabelLength)
			xAxisLabelLength = dataSet->GetLabel().GetLength();
	}
	if(numData > seriesSize)
		seriesSize = numData;

	graphSeries->AddTail(dataSet);
}
#else
void CGraph::AddSeries(CGraphSeries *dataSet)
{
	if(dataSet->GetLabel().GetLength() > xAxisLabelLength)
			xAxisLabelLength = dataSet->GetLabel().GetLength();
	graphSeries->AddTail(dataSet);
}
#endif

void CGraph::SetXAxisLabel(CString label)
{
	axisXLabel = label;
}

void CGraph::SetYAxisLabel(CString label)
{
	axisYLabel=label;
}

void CGraph::SetColor(int group,COLORREF groupColor)
{
	dataColor[group] = groupColor;
}

COLORREF CGraph::GetColor(int group)
{
	return dataColor[group];
}

void CGraph::DrawSeries(CDC *pDC)
{
	int barWidth;
	int dataPlotSize;   //used to plot rects of data
	int barL, barT, barR, barB;
	int tickXLocation;
	float seriesSpace;
	double barHeight;  //for scalability
	POSITION pos;
	CGraphSeries* tmpSeries;

	int iGraphSeriesCount = graphSeries->GetCount();
	int lastXLoc, lastYLoc;
	for(int s = 0; s < seriesSize; s++)
	{
		//determine width of barchart data blocks
		if(graphHasLegend)
		{	
			//modified by LiuSong
			//seriesSpace = (xAxisWidth - legendWidth - 10) / iGraphSeriesCount;
			seriesSpace = (xAxisWidth - legendWidth - 10) * m_scale / iGraphSeriesCount ;
		}
		else
		{	
			//modified by LiuSong
			//seriesSpace = xAxisWidth / graphSeries->GetCount();
			seriesSpace = 1.00;
			seriesSpace	*= xAxisWidth;
			seriesSpace *= m_scale;
			seriesSpace	/= iGraphSeriesCount;
		}

		barWidth = (seriesSpace * .6) / seriesSize;
		dataPlotSize = seriesSize * barWidth;

		int xLabelStep = (16 / seriesSpace);
		int xAxisStep  = (4  / seriesSpace);
		int nStartx = iGraphSeriesCount - (iGraphSeriesCount / m_scale);

		pos = graphSeries->GetHeadPosition();
		for(int ii = 1; ii < nStartx ;ii++)
		{
			tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);
		}

		for(int nDataCount = 0; nDataCount < (iGraphSeriesCount / m_scale); nDataCount++)
		{
			tmpSeries = (CGraphSeries*)graphSeries->GetNext(pos);

			//if( xLabelStep < 2 || !(nDataCount % xLabelStep) )
			if( xAxisStep < 2 || !(nDataCount % xAxisStep) )
			{
				/* modified by LiuSong
				tickXLocation = xApexPoint + ((x * seriesSpace) - (seriesSpace / 2));
				*/
				float fLen;
				fLen = (nDataCount * seriesSpace) + (seriesSpace / 2);
				tickXLocation = xApexPoint;
				tickXLocation += (int)fLen;

				barHeight = 0.00;
				//to allow scalability (height may be less than tickRange)
				double dataScale = 0.00;
				dataScale = (yAxisHeight * 1.00) / (tickRange * 1.00);

				barHeight = (tmpSeries->GetData(s) * 1.00) * dataScale;

				int yDataLocation = yApexPoint - barHeight;
				
				//now have x and y location of center of ellipse
				COLORREF barColor;
				//modified by LiuSong
				//barColor = GetColor(s);
				barColor = GetColor(GetCurColorIndex());

				CBrush brush (barColor);
				CBrush* pOldBrush;
				pOldBrush = pDC->SelectObject(&brush);

				//draw line back to last data member
				if(nDataCount > 0)
				{
					CPen* pOldPen;
					CPen linePen (PS_SOLID, 1, barColor);
					pOldPen = pDC->SelectObject(&linePen);
					pDC->MoveTo(lastXLoc + 2, lastYLoc - 1);
					pDC->LineTo(tickXLocation - 3, yDataLocation - 1);
					pDC->SelectObject(pOldPen);
				}

				//now draw ellipse...
				pDC->Ellipse(tickXLocation - 3, yDataLocation - 3,tickXLocation + 3, yDataLocation + 3);
				lastXLoc = tickXLocation;
				lastYLoc = yDataLocation;
				pDC->SelectObject(&pOldBrush);
			}
		}
	}
}

void CGraph::SetLegend(int datagroup,CString label)
{
	/* disabled by LiuSong
	graphLegend.SetLegendText(datagroup, label);
	graphHasLegend = TRUE;
	*/
}

void CGraph::DrawLegend(CDC *pDC)
{
	//determine size of legend
	//12 chars per seriesSize + 6 for spacing (3 top and bottom) 
	//+ 1 set for label title(3+12+6) + rectangle (2 chars) + 3 for final bottom buffer
	int legendHeight = 23 + (seriesSize * 18) + 3;
	int legendL, legendT, legendR, legendB;
	int barL, barT, barR, barB;

	legendT = (maxHeight / 2) - (legendHeight / 2);
	legendB = legendT + legendHeight;
	legendR = maxWidth - 5;
	legendL = legendR - ((graphLegend.GetLegendLength() * 12) + 50);
		//allows 50 pixels for display of legend bar 45 + 5 space.
	legendWidth = legendR - legendL;

	pDC->Rectangle(legendL, legendT, legendR, legendB);
	pDC->TextOut(legendL + (legendWidth / 2) - 24, legendT + 3, Translate(_T("Legend")));
	for(int i = 0; i < seriesSize; i++)
	{
		//top "Legend" text will use 12 + 3 top + 6 bottom (21 total)
		//each legend label will need 3 chars on top, so the 24 in the offset
		//each label than uses 12 + 3 below + 3 above next label, so 18
		// in the i * offset.  
		pDC->TextOut(legendL + 5, legendT + 24 + (i * 18), graphLegend.GetLegendText(i));

		//draw bar
		COLORREF barColor;
		barColor = GetColor(i);
		CBrush brush (barColor);
		CBrush* pOldBrush;
		pOldBrush = pDC->SelectObject(&brush);

		barL = legendL + 5 + (graphLegend.GetLegendText(i).GetLength() * 8) + 5;
		barT = legendT + 24 + (i * 18) + 1, graphLegend.GetLegendText(i);
		barR = legendR - 5;
		barB = barT + 12;
		pDC->Rectangle(barL, barT, barR, barB);

		pDC->SelectObject(&pOldBrush);
	}
}

void CGraph::SetGraphTitle(CString title)
{
	graphTitle = title;
}

#endif