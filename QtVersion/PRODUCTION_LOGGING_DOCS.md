# Production Logging Feature - Documentation

## Overview

The production logging system automatically tracks production runs and provides a convenient way to view and export production history.

## Features

### Automatic Logging
- **When**: Automatically logs production data when counter resets (TimeDelay state completion)
- **What's Logged**:
  - Date and Time
  - Production Count
  - Speed Setting (1-6)
  - Tray Name
  - Total Counter Value
- **Storage**: CSV file (`ProductionLog.csv`) in application directory

### Log Viewer
Access via: **Edit Menu → View Production Log**

#### Features:
1. **Table View**
   - Displays all production runs (newest first)
   - Sortable columns
   - Alternating row colors for readability
   - Shows: Date/Time, Count, Speed, Tray, Total Counter

2. **Statistics**
   - Average count per run
   - Date range of logged data
   - Total number of entries

3. **Actions**
   - **Refresh**: Reload log data from file
   - **Export to CSV**: Save log to custom location
   - **Clear Log**: Delete all log entries (with confirmation)
   - **Close**: Close viewer window

### Data Format

**CSV Structure:**
```csv
Timestamp,Count,Speed,Tray,TotalCounter
2025-12-22 14:30:45,150,3,Tray 1,5432
2025-12-22 15:15:20,200,4,Tray 2,5632
```

## Usage

### Viewing Production History

1. Click **Edit** menu
2. Select **View Production Log**
3. Browse production history in the table
4. View statistics at the top

### Exporting Data

1. Open Log Viewer
2. Click **Export to CSV**
3. Choose save location and filename
4. Open in Excel, Google Sheets, or any CSV viewer

### Clearing Old Data

1. Open Log Viewer
2. Click **Clear Log** button (orange/red)
3. Confirm deletion
4. Log file will be reset with just headers

**⚠️ Warning:** Clearing cannot be undone! Export data first if you need to keep it.

## Technical Details

### Files
- **ProductionLog.h/cpp**: Log data management class
- **logviewer.h/cpp/ui**: Log viewer dialog
- **ProductionLog.csv**: CSV file containing all log data

### Integration Points
- **TimeDelay.cpp**: `StateTimeDelayTimerComplete()` triggers logging before counter reset
- **mainwindow.cpp**: `logProductionRun()` performs the actual logging

### Data Safety
- Log appends to file immediately when entry is added
- No buffering - each production run is written instantly
- Survives application crashes and power failures
- Data persists across application restarts

## Troubleshooting

### Log Not Updating
- Click **Refresh** button in Log Viewer
- Check that production runs are completing (counter resets)
- Verify `ProductionLog.csv` file exists in application directory

### Export Failed
- Check write permissions for destination folder
- Ensure file is not open in another program
- Try different filename/location

### Old Data Not Showing
- Log Viewer loads from CSV file on open
- If file was deleted/moved, a new empty log will be created
- Check application directory for `ProductionLog.csv`

## Best Practices

1. **Regular Exports**
   - Export log monthly for archival
   - Keep backups of important production data
   - Use date in export filename (e.g., `ProductionLog_Dec2025.csv`)

2. **Periodic Cleanup**
   - Clear very old data to keep file size manageable
   - Archive old data before clearing

3. **Data Analysis**
   - Export to Excel for charts and pivot tables
   - Track average production rates by tray type
   - Monitor production trends over time

## Future Enhancements

Potential features for future versions:
- Filter by date range
- Filter by tray type
- Charts and graphs
- Automatic monthly archiving
- Database storage option
- Search functionality
- Print reports

---

**Note**: Production logging is separate from the total counter file. The counter file tracks the overall count, while the production log tracks individual production runs with full context (speed, tray, etc.).
