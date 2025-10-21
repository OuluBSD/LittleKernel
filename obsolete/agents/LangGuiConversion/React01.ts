
// 1. Hello World GUI (React + MUI)
// 2. Simple button with click -> alert
import React from 'react';
import { Box, Button, Typography } from '@mui/material';

export function React01_Hello() {
  return (
    <Box sx={{ width: 320, height: 240, display: 'flex', alignItems: 'center', justifyContent: 'center' }}>
      <Typography variant="body1">Hello world!</Typography>
    </Box>
  );
}

export function React01_Button() {
  return (
    <Box sx={{ width: 320, height: 240, position: 'relative' }}>
      <Button
        variant="contained"
        sx={{ position: 'absolute', left: 30, top: 30, width: 100, height: 30 }}
        onClick={() => alert('Popup message')}
      >
        Hello world!
      </Button>
    </Box>
  );
}
